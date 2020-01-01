#include <libwebsockets.h>

const int PROTOCOL_WS_ASCII = 1;

const std::string VT_CURSOR_FORWARD = "\x1b[C";
const std::string VT_CURSOR_BACK = "\x1b[D";

/* one of these is created for each client connecting to us */

struct LineState {
  // Total number of cols on screen
  uint32_t cols;

  // Cursor position in column
  uint32_t cur;

  // cursor pointing to character index
  uint32_t index;

  // the current line buffer
  std::string buf;

  // the current echo back buffer
  std::string echo;

  bool need_clear_line = false;

  LineState() {
    this->cols = 80 - 2; // prompt
    this->cur = 0;
    this->index = 0;
    this->echo.reserve(this->cols);
    this->buf.reserve(1024);
  }

  void forward() {
    if (this->cur == this->buf.length()) {
      return ;
    }
    if (this->cur == this->cols - 1) {
      return;
    }
    this->cur += 1;
    this->echo += VT_CURSOR_FORWARD;
  }

  void back() {
    if (this->cur == 0) {
      return;
    }
    this->cur -= 1;
    this->echo += VT_CURSOR_BACK;
  }

  bool insert(char c) {
    // executing command.
    if (c == '\r') {
      this->buf.push_back('\r');
      this->buf.push_back('\n');
      this->echo.push_back('\r');
      this->echo.push_back('\n');
      this->cur = 0;
      return true;
    }

    auto pos = this->cur;

    if (pos == this->cols - 1) {
      // don't accept more chars.
      return false;
    }

    this->cur += 1;

    // pointing to the end
    if(pos != this->buf.length()) {
      // doing an insert, refresh line
      this->need_clear_line = true;
    }

    this->buf.insert(pos, 1, c);
    if (this->buf.length() > this->cols) {
      // truncate
      this->buf = this->buf.substr(0, this->cols);
      this->need_clear_line = true;
    }
    this->echo.push_back(c);

    return false;
  }

  void backspace() {
    auto pos = this->cur;

    if (pos != 0) {
      this->cur -= 1;
      this->buf.erase(pos - 1, 1);
      this->need_clear_line = true;
    }
  }

  void del() {
    auto pos = this->cur;

    if (pos != this->buf.length()) {
      this->buf.erase(pos, 1);
      this->need_clear_line = true;
    }
  }

  void home() {
    this->cur = 0;
    this->need_clear_line = true;
  }

  void end() {
    this->cur = std::min(this->buf.length(), (size_t)(this->cols-1));
    this->need_clear_line = true;
  }

  std::string print_echo(const char* prompt) {
    std::string res;
    res = this->echo;
    this->echo.clear();

    if (this->need_clear_line) {
      this->need_clear_line = false;
      res = "";
      res += "\r";  // to left edge
      res += "\x1b[2K";  // clearline
      res += prompt;  // prompt
      res += this->buf;  // line buffer
      // TODO: optimize for less movement
      res += "\r"; // to left edge again
      res += VT_CURSOR_FORWARD;
      res += VT_CURSOR_FORWARD; // prompt
      for(auto i=0;i < this->cur; i++) {
        res += VT_CURSOR_FORWARD;
      }
    }
    return res;
  }
};

struct ws_ascii_session {
  struct lws *wsi;
  struct interactive_t *user;

  struct evbuffer *buffer;

  LineState* line;
};

int ws_ascii_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in,
                      size_t len);

void ws_ascii_send(struct lws* wsi, const char* data, size_t len);
