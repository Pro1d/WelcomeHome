#pragma once

#include "Arduino.h"

#define SNAKE_PERIOD  250
#define SNAKE_INITIAL_SIZE  2
#define SNAKE_SIZE_MAX  16 // power of 2 only
#define SNAKE_SIZE_MAX_MASK  (SNAKE_SIZE_MAX-1)
#define SNAKE_EVALUATE_INFINITE 0xffffu

struct Pos {
  Pos() = default;
  Pos(const Pos&) = default;
  Pos(int r, int c) : r(r), c(c) {}
  Pos operator+(const Pos& b) const {
    return Pos(r + b.r, c + b.c);
  }
  Pos rotateLeft() const {
    return Pos(-c, r);
  }
  Pos rotateRight() const {
    return Pos(c, -r);
  }
  static int distance(const Pos& a, const Pos& b) {
    return abs(a.r - b.r) + abs(a.c - b.c);
  }
  bool operator==(const Pos& a) const {
    return r == a.r && c == a.c;
  }
  int r, c;
};

template <int ROW, int COL>
class SnakeAnimation {
  public:
    void init() {
      last_frame = 0;
      animation_frame = 0;
      best_score = 0;
      reset();
    }

    template <const int _RO[ROW], const int _CO[COL]>
    void update(Matrix<ROW, COL, _RO, _CO>& matrix) {
      unsigned long t = millis();
      if(t - last_frame < SNAKE_PERIOD)
        return;

      last_frame = t;

      animation_frame++;
      if((animation_frame & 1) == 0) {
        matrix.clear(apple.r, apple.c);
        return;
      }

      // remove tail from snake_grid
      const Pos& tail = snake[get_snake_index(snake_size - 1)];
      clear_grid_at(tail);

      // Evaluate each direction
      const Pos& left = snake_dir.rotateLeft();
      const Pos& right = snake_dir.rotateRight();
      const unsigned int eval_left = evaluate_dir(left);
      const unsigned int eval_right = evaluate_dir(right);
      unsigned int best_eval = evaluate_dir(snake_dir); // default best dir is forward

      // Choose best direction
      if(eval_left < best_eval) {
        if(eval_right < eval_left) {
          snake_dir = right;
          best_eval = eval_right;
        } else {
          snake_dir = left;
          best_eval = eval_left;
        }
      } else if(eval_right < best_eval) {
        snake_dir = right;
        best_eval = eval_right;
      }

      // Check direction is valid
      if(best_eval == SNAKE_EVALUATE_INFINITE) {
        // GAME OVER!
        playing = false;
        if(score > best_score)
          best_score = score;
        // reset();
      }
      else {
        // update snake
        move();
        if(snake[snake_offset] == apple) {
          score++;
          // restore tail
          set_grid_at(tail);
          if(snake_size < SNAKE_SIZE_MAX)
            snake_size++;
          // new apple
          spawn_apple();
        }
      }

      // update display
      matrix.draw(snake_grid);
      matrix.set(apple.r, apple.c);
    }
    bool is_playing() const {
      return playing;
    }
    int get_score() const {
      return score;
    }
    int get_best_score() const {
      return best_score;
    }
    void reset() {
      score = 0;
      playing = true;
      // Direction: toward right
      snake_dir.r = 0;
      snake_dir.c = 1;
      snake_size = SNAKE_INITIAL_SIZE;
      snake_offset = SNAKE_INITIAL_SIZE - 1;
      // Snake centered, straight
      snake[0].r = ROW / 2 - snake_dir.r * SNAKE_INITIAL_SIZE / 2;
      snake[0].c = COL / 2 - snake_dir.c * SNAKE_INITIAL_SIZE / 2;
      for(int i = 1; i < SNAKE_INITIAL_SIZE; i++)
        snake[i] = snake[i - 1] + snake_dir;
      redraw_snake_grid();
      spawn_apple();
    }
  private:
    void move() {
      unsigned int next_index = get_snake_index(-1);
      snake[next_index] = snake[snake_offset] + snake_dir;
      set_grid_at(snake[next_index]);
      snake_offset = next_index;
    }
    unsigned int evaluate_dir(const Pos& dir) {
      Pos p = snake[snake_offset] + dir;
      if(p.r < 0 || p.r >= ROW || p.c < 0 || p.c >= COL
          || get_grid_at(p))
        return SNAKE_EVALUATE_INFINITE;
      
      return Pos::distance(p, apple);
    }

    // pos=0 for head, pos=snake-1 for tail
    unsigned int get_snake_index(int pos) {
      return (snake_offset - pos) & SNAKE_SIZE_MAX_MASK;
    }

    void redraw_snake_grid() {
      // clear snake_grid
      for(int r = 0; r < ROW; r++)
        snake_grid[r] = 0x0;
      // draw snake
      for(unsigned int s = 0; s < snake_size; s++) {
        unsigned int index = get_snake_index(s);
        set_grid_at(snake[index]);
      }
    }
    
    void spawn_apple() {
      do {
        apple.r = random(ROW);
        apple.c = random(COL);
      } while(get_grid_at(apple));
    }

    bool get_grid_at(const Pos& p) {
      return (snake_grid[p.r] >> p.c) & 0x1;
    }
    void set_grid_at(const Pos& p) {
      snake_grid[p.r] |= (1 << p.c);
    }
    void clear_grid_at(const Pos& p) {
      snake_grid[p.r] &= ~(1 << p.c);
    }

    unsigned int animation_frame;
    unsigned long last_frame;
    Pos snake_dir;
    Pos snake[SNAKE_SIZE_MAX];
    Pos apple;
    unsigned int score, best_score;
    unsigned int snake_size, snake_offset;
    byte snake_grid[ROW];
    bool playing;
};
