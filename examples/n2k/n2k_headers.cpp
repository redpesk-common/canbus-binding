#include <utils/signals.hpp>
#include <utils/openxc-utils.hpp>
class pgn_60416_BAM_t
{
private:
    int code_;
    int size_;
    int packets_;
    pgn_t pgn_;
    uint8_t reserved_;
    int sub_id_;

public:
    pgn_60416_BAM_t(){};
    pgn_60416_BAM_t(int code, int size, int packets, pgn_t pgn, uint8_t reserved, int sub_id)
    {
        code_ = code;
        size_ = size;
        packets_ = packets;
        pgn_ = pgn;
        reserved_ = reserved;
        sub_id_ = sub_id;
    };
    int get_code()
    {
        return code_;
    }
    int get_size()
    {
        return size_;
    }
    int get_packets()
    {
        return packets_;
    }
    pgn_t get_pgn()
    {
        return pgn_;
    }
    int get_sub_id()
    {
        return sub_id_;
    }
};

class pgn_60160_t
{
private:
    int sid_;
    uint8_t data_[8];

public:
    pgn_60160_t(){};
    pgn_60160_t(int sid, uint8_t data[])
    {
        sid_ = sid;
        //data_ = [8];
        for (int i = 0; i < 8; i++)
            data_[i] = data[i];
    };
    int get_sid()
    {
        return sid_;
    };
    uint8_t *get_data()
    {
        return data_;
    };
};

class stack_message_t
{
private:
    static std::vector<pgn_60416_BAM_t> stack_60416_;
    static std::vector<pgn_60160_t> stack_60160_;
    application_t *app_;

public:
    stack_message_t(application_t *app)
    {
        app_ = app;
    };
    static stack_message_t &instance(application_t *app)
    {
        static stack_message_t stack(app);
        return stack;
    }
    ~stack_message_t(){};
    std::vector<pgn_60416_BAM_t> *get_stack_60416()
    {
        return &stack_60416_;
    };
    std::vector<pgn_60160_t> *get_stack_60160()
    {
        return &stack_60160_;
    };

    std::map<std::string, openxc_DynamicField> translate_message(std::shared_ptr<message_t> message, pgn_t pgn)
    {
        vect_ptr_msg_def_t messages_definition = app_->get_messages_definition(pgn);
        if (messages_definition.empty())
            return std::map<std::string, openxc_DynamicField>();

        bool send = true;

        openxc_DynamicField dynamicField_tmp;

        for (std::shared_ptr<message_definition_t> message_definition : messages_definition)
        {

            bool valid = true;
            std::map<std::string, openxc_DynamicField> tmp = std::map<std::string, openxc_DynamicField>();
            for (std::shared_ptr<signal_t> sig : message_definition->get_signals())
            {
                dynamicField_tmp = decoder_t::translate_signal(*sig, message, &send);
                if (!dynamicField_tmp.has_type)
                {
                    valid = false;
                }
                tmp.insert(std::pair<std::string, openxc_DynamicField>(sig->get_name(), dynamicField_tmp));
            }
            if (valid)
            {
                return tmp;
            }
        }
        return std::map<std::string, openxc_DynamicField>();
    };

    std::shared_ptr<j1939_message_t> get_message()
    {
        int need_nb_packet = 0;
        for (auto pgn_60416 : stack_60416_)
        {
            need_nb_packet += pgn_60416.get_packets();
        }

        if (need_nb_packet == stack_60160_.size())
        {
            pgn_60416_BAM_t last_60416 = stack_60416_.back();
            stack_60416_.pop_back();

            std::vector<pgn_60160_t> lasts_60160 = std::vector<pgn_60160_t>();
            int nb_packets = last_60416.get_packets();
            for (int i = 0; i < nb_packets; i++)
            {
                lasts_60160.push_back(stack_60160_.back());
                stack_60160_.pop_back();
            }

            uint8_t sorted[nb_packets][8];
            for (pgn_60160_t pgn : lasts_60160)
            {
                int sid = pgn.get_sid();
                if (sid > 0 && sid < nb_packets)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        sorted[sid][i] = pgn.get_data()[i];
                    }
                }
            }

            std::vector<uint8_t> data = std::vector<uint8_t>();

            for (int i = 0; i < nb_packets; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    data.push_back(sorted[i][j]);
                }
            }

            std::shared_ptr<j1939_message_t> jm = std::make_shared<j1939_message_t>(j1939_message_t(last_60416.get_size(),
                                                                                                    data,
                                                                                                    12,
                                                                                                    1,
                                                                                                    last_60416.get_pgn(),
                                                                                                    1));

            return jm;
        }

        return std::make_shared<j1939_message_t>(j1939_message_t());
    }
};

std::vector<pgn_60416_BAM_t> stack_message_t::stack_60416_ = std::vector<pgn_60416_BAM_t>();
std::vector<pgn_60160_t> stack_message_t::stack_60160_ = std::vector<pgn_60160_t>();

openxc_DynamicField decoder_t::decode_60416(signal_t &signal, std::shared_ptr<message_t> message, bool *send)
{
    application_t *app = &application_t::instance();
    stack_message_t stack = stack_message_t::instance(app);
    auto res = stack.translate_message(message, 60416);
    if (res.size() != 0)
    {
        pgn_60416_BAM_t pgn_60416_BAM = pgn_60416_BAM_t(
            (int)res.find("messages.Iso.Transport.Protocol.Connection.Management.Broadcast.Announce.Group.Function.Code")->second.numeric_value,
            (int)res.find("messages.Iso.Transport.Protocol.Connection.Management.Broadcast.Announce.Message.Size")->second.numeric_value,
            (int)res.find("messages.Iso.Transport.Protocol.Connection.Management.Broadcast.Announce.Packets")->second.numeric_value,
            (int)res.find("messages.Iso.Transport.Protocol.Connection.Management.Broadcast.Announce.Pgn")->second.numeric_value,
            res.find("messages.Iso.Transport.Protocol.Connection.Management.Broadcast.Announce.Reserved")->second.bytes_value[0],
            message->get_sub_id());
        stack.get_stack_60416()->push_back(pgn_60416_BAM);
    }

    openxc_DynamicField ret = build_DynamicField_error();
    return ret;
}

openxc_DynamicField decoder_t::decode_60160(signal_t &signal, std::shared_ptr<message_t> message, bool *send)
{
    application_t *app = &application_t::instance();
    stack_message_t stack = stack_message_t::instance(app);
    auto res = stack.translate_message(message, 60160);
    if (res.size() != 0)
    {
        pgn_60160_t pgn_60160 = pgn_60160_t(
            (int)res.find("messages.Iso.Transport.Protocol.Data.Transfer.Sid")->second.numeric_value,
            res.find("messages.Iso.Transport.Protocol.Data.Transfer.Data")->second.bytes_value);
        stack.get_stack_60160()->push_back(pgn_60160);
    }

    std::shared_ptr<j1939_message_t> jm = stack.get_message();

    if (!(jm->get_flags() & J1939_PROTOCOL))
        return build_DynamicField_error();

    utils::signals_manager_t &sm = utils::signals_manager_t::instance();
    map_subscription &s = sm.get_subscribed_signals();

    for (auto it : s)
    {
        if (it.second->get_message_definition()->get_id() == jm->get_pgn())
            jm->set_sub_id(it.first);
    }

    app->get_can_bus_manager().push_new_can_message(jm);

    return build_DynamicField_error();
}
