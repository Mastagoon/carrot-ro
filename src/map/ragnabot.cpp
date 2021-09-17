
#include "ragnabot.hpp"
#include <stdlib.h>
#include <boost/algorithm/string/predicate.hpp>
#include <nlohmann/json.hpp>
#include "../common/showmsg.hpp"
#include "clif.hpp"
#include "channel.hpp"

std::unique_ptr<ragnabot::socket> sock;

// using namespace ragnabot;
typedef std::chrono::duration<int, std::micro> dur_type;

void ragnabot_init()
{
    sock = std::unique_ptr<ragnabot::socket>(new ragnabot::socket());
}

void send_discord_message(const char *msg, const char *channel, const char *name)
{
    if (!sock)
    {
        ShowWarning("Ragnabot socket is not running.\n");
        return;
    }
    if (!msg || !channel || !name)
    {
        ShowError("send_discord_message called with wrong arguments");
        return;
    }
    std::string msg_str = msg;
    std::string channel_str = channel;
    std::string name_str = name;
    if (msg_str.find('|') == std::string::npos && channel_str.find('|') == std::string::npos && name_str.find('|') == std::string::npos)
        sock->discord_message(msg, channel, name);
}

void discord_verify_char(struct map_session_data *sd, const char *discord_tag)
{
    if (!sd || !discord_tag)
        return;
    if (!sock)
    {
        ShowWarning("Ragnabot socket is not running.\n");
        return;
    }
    std::string args = discord_tag;
    args.append("|");
    args.append(sd->status.name);
    args.append("|");
    args.append(std::to_string(sd->status.char_id));
    args.append("|");
    args.append(std::to_string(sd->status.account_id));
    sock->discord_command("verify", args);
}

void discord_send_whisper(struct map_session_data *sd, const char *message)
{
    if (!sd || !message)
        return;
    if (!sock)
    {
        ShowWarning("Ragnabot socket is not running.\n");
        return;
    }
    std::string args = std::to_string(sd->status.discord_id);
    args.append("|");
    args.append(message);
    args.append("|");
    args.append(std::to_string(sd->status.char_id));
    sock->discord_command("whisper", args);
}

namespace ragnabot
{

    socket::socket()
    {
        c.lock();
        this->socket_thr = std::thread(&socket::run, this);
    }

    // std::chrono::time_point<std::chrono::system_clock> #WUTZIS
    std::chrono::time_point<std::chrono::system_clock> socket::getStartTime() { return this->start_time; } // #NEEDNT

    void socket::discord_reward(uint64_t discord_id, t_itemid nameid, int amount, std::string message)
    {
        std::string query = "INSERT INTO `discord_rewards` (`discord_id`,`reward`,`"
    }

    void socket::discord_message(std::string msg, std::string channel, std::string name)
    {
        if (!this->socket_open)
        {
            ShowError("Cannot send to discord, socket closed.\n");
            return;
        }
        // std::string str = "{\"type\":\"message\",\"data\":\"hello response\"}\f";
        std::string str = "{\"type\":\"message\",\"data\":\"" + msg + "|" + channel + "|" + name + "\"}\f";
        sock->emit(str);
    }

    void socket::discord_command(std::string command, std::string args)
    {
        if (!this->socket_open)
        {
            ShowError("Cannot send to discord, socket closed.\n");
            return;
        }
        // std::string str = "{\"type\":\"message\",\"data\":\"hello response\"}\f";
        std::string str = "{\"type\":\"command\",\"data\":\"" + command + "|" + args + "\"}\f";
        sock->emit(str);
    }

    void socket::do_shutdown() { 
        if(this->heartbeat_active) {
            this->heartbeat_active = false;
            ShowDebug("Waiting for heartbeat thread to finish...\n");
            this->hb_thr.join();
            ShowDebug("Heartbeat thread successfully closed.\n");
            this->started = false;
            // auto event_ptr = std::bind(&core::handle_close, std::placeholders::_1);? #WUTZIS
            std::lock_guard<std::mutex> lock(m);    // not this.m?
        }
    }

    void socket::on_open() {
        ShowInfo("Ragnabot socket opened successfully.\n");
        this->start_time = std::chrono::system_clock::now();
        this->socket_open = true;
        c.unlock();
    }

    void socket::on_fail() {
        ShowError("Ragnabot: socket failed.\n");
        this->do_shutdown();
        if(!this->socket_open)
            c.unlock();
    }

    void socket::on_close() {
        c_close = std::chrono::high_resolution_clock::now();
        // ShowInfo("Socket Init: ")    #TODO
        ShowInfo("Socket closed.\n");
        this->do_shutdown();
    }

    void socket::on_socket_init() {
        c_socket_init = std::chrono::high_resolution_clock::now();
    }

    void socket::on_message(std::string type, std::string message) {
        ShowDebug("Recieved message: %s\n", message);
        ShowDebug("Recieved type: %s\n", type);   // #TODO
    }

    std::string socket::get_error_message(std::string code)
    {
        if (code == "already_linked")
            return "Verification Failed. This character is already linked to another Discord account.";
        if (code == "no_dm")
            return "Verification Failed. The selected Discord user does not allow Direct Messages.";
        if (code == "not_found")
            return "Verification Failed. User not found.";

        return "Verification Failed.";
    }

    void socket::run()
    {
        if(shutdown)
            return;
        ShowInfo("Socket thread started!\n");
        if ((fd = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            perror("socket error");
            exit(-1);
        }

        // int opt = 1;
        // ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        socket_path = "../ragnasock/tmp/ragnabotipc";
        if (*socket_path == '\0')
        {
            *addr.sun_path = '\0';
            strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
        }
  
             else
       
        {
            strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
            unlink(socket_path);
        }

        if (::bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            perror("bind error");
            exit(-1);
        }

        if (::listen(fd, 5) == -1)
        {
            ShowError("Listen error\n");
            exit(-1);
        }

        this->socket_open = true;
        this->started = true;

        while(true) {
            char buf[1024];
            if ((cl = ::accept(fd, NULL, NULL)) == -1)
            {
                ShowError("Accept error.\n");
                continue;
            }

            while (rc = ::read(cl, buf, sizeof(buf)) > 0)
            {
                // ShowInfo("Read %u bytes: %s\n", rc, buf);
                nlohmann::json j, j2;
                int length;
                char *ptr;
                // auto j;
                // j = nlohmann::json::parse(buf);
                std::string s;
                s = buf;
                auto n = s.find("\f");
                if (n != std::string::npos)
                {
                    s.erase(n, 2);
                }
                size_t end = s.find_last_not_of(" \n\r\t\f\v");
                // if() #TODO size limit check
                try
                {
                    j = nlohmann::json::parse(s.substr(0, end + 1).c_str());
                    std::string str = j["data"];
                    std::string type = j["type"];
                    std::string content, user, channel_name = "#", delimiter = "|";
                    std::vector<std::string> data;
                    int pos;
                    while((pos = str.find(delimiter)) != std::string::npos) {
                        data.push_back(str.substr(0, pos));
                        str.erase(0, pos + delimiter.length());
                    }
                    data.push_back(str);
                    channel_name.append(data[2]);
                    if (type == "message")
                    {
                        Channel *send_channel = channel_name2channel((char *)channel_name.c_str(), NULL, 0);
                        if (!send_channel)
                        {
                            ShowWarning("Ragnabot warning: channel not found!");
                        }
                        else
                        {
                            std::string msg = send_channel->alias;
                            msg.append(" <");
                            msg.append(data[1]); // user name
                            msg.append(">");
                            msg.append(":");
                            msg.append(data[0]);
                            clif_channel_msg(send_channel, msg.c_str(), send_channel->color); // #TODO convert string to utf8 to support arabic text
                        }
                    }
                    if (type == "verify_message")
                    {
                        struct map_session_data *sd = map_charid2sd(std::atoi(data[2].c_str()));
                        if (sd)
                        {
                            if (data[0] == "error")
                            {
                                std::string mes = this->get_error_message(data[1]);
                                clif_messagecolor(&sd->bl, color_table[COLOR_LIGHT_GREEN], mes.c_str(), true, SELF);
                            }
                            if (data[0] == "success")
                            {
                                sd->status.discord_id = std::atoll(data[3].c_str());
                                std::string mes = "Verification success! You have been linked to the Discord user ";
                                mes.append(data[1]);
                                clif_messagecolor(&sd->bl, color_table[COLOR_LIGHT_GREEN], mes.c_str(), true, SELF);
                            }
                        }
                    }
                    if (type == "char_reward")
                    {
                        struct map_session_data *sd = map_charid2sd(std::atoi(data[0].c_str()));
                        if (sd)
                        {
                            int i, count, amount = std::atoi(data[2].c_str());
                            struct item it;
                            struct item_data *id = NULL;
                            t_itemid nameid = std::atoi(data[1].c_str());
                            // check if item exists
                            if (!(id = itemdb_exists(nameid)))
                            {
                                ShowError("ragnabot_char_reward: Nonexistant item %u requested.\n", nameid);
                                return;
                            }
                            memset(&it, 0, sizeof(it));
                            it.nameid = nameid;
                            it.identify = 1;
                            it.bound = BOUND_NONE; // #TODO add option for bound
                            if (!itemdb_isstackable2(id))
                                count = 1;
                            else
                                count = amount;
                            for (i = 0; i < amount; i += count)
                            {
                                // if not pet egg
                                if (!pet_create_egg(sd, nameid))
                                {
                                    if ((flag = pc_additem(sd, &it, count, LOG_TYPE_SCRIPT)))
                                    {
                                        clif_additem(sd, 0, 0, flag);
                                        if (pc_candrop(sd, &it))
                                            map_addflooritem(&it, count, sd->bl.m, sd->bl.x, sd->bl.y, 0, 0, 0, 0, 0);
                                    }
                                }
                            }
                            // #TODO data[4] has a message that can be displayed to user.
                        }
                    }
                }

                catch (nlohmann::json::parse_error &err)

                {
                    ShowError("Ragnabot error: %s\n", err.what());
                }
                memset(&buf, 0, sizeof(buf));
            }
            // info here
            if(rc == -1) {
                perror("read\n");
                exit(-1);
            } else if(rc == 0) {
                ShowInfo("IPC connection closed.");
                close(cl);
            }
        }
    }

    int socket::emit(const void *data, int data_size)
    {
        const char *data_ptr = (const char *)data;
        int bytes_sent;

        while (data_size > 0)
        {
            bytes_sent = ::send(this->cl, data_ptr, data_size, 0);
            if (bytes_sent == -1)
                return -1;
            data_ptr += bytes_sent;
            data_size -= bytes_sent;
        }
        return 1;
    }

    int socket::emit(const std::string &data)
    {
        // ulong data_size = htonl(data.size());
        // int result = SendAll(client_socket, &data_size, sizeof(data_size));
        // if (result == 1)
        int result = this->emit(data.c_str(), data.size());
        return result;
    }

    void socket::start_heartbeat(int interval) {
        this->heartbeat_interval = interval;
        this->heartbeat_active = true;
        this->hb_thr = std::thread(&socket::do_heartbeat, this);
    }

    void socket::do_heartbeat()
    {
        ShowInfo("Heartbeat thread started...");
        if ((fd = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            ShowError("Socket Error!\n");
            exit(-1);
        }

        if (::bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            ShowError("Bind error\n");
            exit(-1);
        }

        if (::listen(fd, 5) == -1)
        {
            ShowError("Listen error\n");
            exit(-1);
        }
    }

        // while(this->heartbeat_active) {
            
        // }
        // json heartbeat;
        // while(this->heartbeat_active) {
        //     heartb = { { "op", 1 }, { "d", this->sequence_number } };
        //     if(fd = socket(AF_UNIX, SOCK_STREAM, 0) <0)
        //         throw std::runtime_error("cannot create socket");
        //     int opt = 1;
        //     setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        //     if (bind(fd, (struct sockaddr*)(addr), sizeof(server)) < 0) 
        //         throw std::runtime_error("cannot bind socket");
        //     if(listen(fd, 64) < 0)
        //         throw std::runtime_error("cannot start listening");
        //     while(true) {
        //         struct sockaddr_in client;

    //     }
    // }
    // }

    socket::~socket() {
        c.lock();
        ShowInfo("Ragnabot is shutting down!");
        if(this->started) {
            ::close(fd);
            ShowInfo("Connection closed");
        }
        this->shutdown = true;
        c.unlock();
        if(socket_thr.joinable())
            this->socket_thr.join();
    }
}