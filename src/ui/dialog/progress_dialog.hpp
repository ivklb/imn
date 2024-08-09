
#ifndef UI__DIALOG__PROGRESS_DIALOG_HPP
#define UI__DIALOG__PROGRESS_DIALOG_HPP

#include <imgui.h>

#include <functional>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

class ProgressDialog {
   public:
    ProgressDialog();
    static ProgressDialog* Instance() {
        static ProgressDialog _instance;
        return &_instance;
    }

    void set_title(const std::string& title) { _title = title; }
    void set_max_v(int max_v) { _max_v = max_v; }
    void set_current(int current) { _current = current; }
    void set_cancel_callback(std::function<void()> callback) { _cancel_callback = callback; }
    void cancel();
    void render();

   private:
    bool _show;
    std::mutex _mutex;
    std::string _title;
    int _max_v;
    int _current;
    std::function<void()> _cancel_callback;
};

#endif
