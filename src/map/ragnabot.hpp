#ifndef RAGNABOT_HPP
#define RAGNABOT_HPP

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <chrono>
#include <mutex>
#include <string>
#include <stdio.h>
#include <queue>
// #include <functional>
#include <thread>
#include "pc.hpp"

void ragnabot_init();
void send_discord_message(const char *msg, const char *channel, const char *name);
void discord_verify_char(struct map_session_data *sd, const char *discord_tag);
void discord_send_whisper(struct map_session_data *sd, const char *message);

struct discord_user_data
{
    std::string discord_id;
};

namespace ragnabot
{
    class socket
    {
    public:
        socket();
        ~socket(); // #TODO why?
        // void void ();
        void start_heartbeat(int interval);
        std::chrono::time_point<std::chrono::system_clock> getStartTime();
        void discord_message(std::string msg, std::string channel, std::string name);
        void discord_command(std::string command, std::string args);
        int emit(const std::string &data);

    private:
        void socket_init();
        void on_message(std::string type, std::string message);
        void on_close();
        void on_fail();
        void on_open();
        void on_socket_init();
        void do_shutdown();
        void run();
        int emit(const void *data, int data_size);
        std::chrono::high_resolution_clock::time_point c_start;
        std::chrono::high_resolution_clock::time_point c_socket_init;
        std::chrono::high_resolution_clock::time_point c_close;
        std::chrono::time_point<std::chrono::system_clock> start_time;
        bool heartbeat_active = false;
        bool shutdown = false;
        bool started = false;
        bool socket_open = false;
        void do_heartbeat();
        int heartbeat_interval;
        std::thread hb_thr;
        std::thread socket_thr;
        // std::queue<std::function<vod
        // std::queue event_queue;
        std::mutex m;
        std::mutex c;
        int fd, cl, rc;
        // std::string addr;
        struct sockaddr_un addr;
        const char *socket_path;
        std::string get_error_message(std::string);
        void discord_reward(uint64_t discord_id, t_itemid nameid, int amount, std::string message);
    };
}

#endif