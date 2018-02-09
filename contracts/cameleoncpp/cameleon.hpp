#pragma once

#include <eosiolib/chain.h>
#include <eosiolib/dispatcher.hpp>
#include <eosiolib/table.hpp>
#include <eosiolib/db.hpp>
#include <eosiolib/vector.hpp>
#include <eosiolib/string.hpp>
#include <eosiolib/action.hpp>

namespace cameleon {
  using eosio::action_meta;
  using eosio::vector;
  using eosio::string;

  template<uint64_t DeployToAccount>
  class contract {
    public:
      static const uint64_t code = DeployToAccount;
      typedef uint64_t account_name;

      struct sethistory : public action_meta<N(eosio),N(sethistory)> {
        string datapath;
        account_name owner;
        string enckey;
        vector<string> readers;

        template<typename DataStream>
        friend DataStream& operator << ( DataStream& ds, const sethistory& t ){
          return ds << t.datapath << t.owner << t.enckey << t.readers;
        }

        template<typename DataStream>
        friend DataStream& operator >> ( DataStream& ds,  sethistory& t ){
          return ds >> t.datapath >> t.owner >> t.enckey >> t.readers;
        }
      };

      struct historyrow {
        string datapath;
        account_name owner;
        string enckey;
        vector<string> readers;

        template<typename DataStream>
        friend DataStream& operator << ( DataStream& ds, const historyrow& t ){
          return ds << t.datapath << t.owner << t.enckey << t.readers;
        }

        template<typename DataStream>
        friend DataStream& operator >> ( DataStream& ds,  historyrow& t ){
          return ds >> t.datapath >> t.owner >> t.enckey >> t.readers;
        }
      };

      struct demand : public action_meta<N(eosio),N(demand)> {
        account_name demander;
        string datapath;
        uint64_t bid;

        template<typename DataStream>
        friend DataStream& operator << ( DataStream& ds, const demand& t ){
            return ds << t.demander << t.datapath << t.bid;
        }

        template<typename DataStream>
        friend DataStream& operator >> ( DataStream& ds,  demand& t ){
            return ds << t.demander << t.datapath << t.bid;
        }
      };

      struct demandrow {
        string id;
        uint64_t bid;

        template<typename DataStream>
        friend DataStream& operator << ( DataStream& ds, const demandrow& t ){
          return ds << t.id << t.bid;
        }

        template<typename DataStream>
        friend DataStream& operator >> ( DataStream& ds,  demandrow& t ){
          return ds >> t.id >> t.bid;
        }
      };

      struct approve : public action_meta<N(eosio),N(approve)> {
        account_name account;
        string datapath;

        template<typename DataStream>
        friend DataStream& operator << ( DataStream& ds, const approve& t ){
          return ds << t.account << t.datapath;
        }

        template<typename DataStream>
        friend DataStream& operator >> ( DataStream& ds,  approve& t ){
          return ds >> t.account >> t.datapath;
        }
      };

      static eosio::var_table<N(cameleon), N(cameleon), N(histories), code, char*>  histories_table;
      static eosio::var_table<N(cameleon), N(cameleon), N(demands), code, char*>  demands_table;

      static string construct_demand_id(account_name demander, string datapath) {
        uint64_t demander_account = demander;
        int digitCnt = 0;
        while(demander_account > 0) {
          demander_account /= 10;
          digitCnt++;
        }


        string demander_part = string();
        demander_account = demander;
        for (int i = 0; i < digitCnt; i++) {
          char ch = '0' + (demander_account % 10);
          demander_part = string(&ch) + demander_part;
        }

        return datapath + string("_") + demander_part;
      }

      static demand demandrow_to_demand(demandrow& dr) {
        char *id_data = dr.id.get_data();
        size_t id_size = dr.id.get_size();

        int bar_idx = 0;
        while(id_data[bar_idx] != '_') {
          bar_idx++;
        }

        demand d;
        d.datapath = string(id_data).substr(0, bar_idx, true);
        string demander_part = string(id_data).substr(bar_idx + 1, id_size - bar_idx - 1, true);
        char *demander_data = demander_part.get_data();

        uint64_t demander = 0;
        for (int i = 0; i < demander_part.get_size(); i++) {
          demander *= 10;
          demander += demander_data[i] - '0';
        }

        d.demander = demander;
        d.bid = dr.bid;

        return d;
      }

      static void on(const sethistory& h) {
        require_auth(h.owner);
        historyrow hr = historyrow{
          .datapath = h.datapath,
          .owner = h.owner,
          .enckey = h.enckey,
          .readers = h.readers
        };

        histories_table.store((char*)hr.datapath.get_data(), hr.datapath.get_size(), (char*)&hr, sizeof(hr));
      }

      static void on(const demand& d) {
        require_auth(d.demander);

        historyrow h;
        histories_table.load((char*)d.datapath.get_data(), d.datapath.get_size(), (char*)&h, sizeof(h));;
        if (h.datapath.get_size() == 0) {
          require_recipient(h.owner, d.demander);
        } else {
          require_recipient(d.demander);
        }

        demandrow dr = demandrow{
          .id = construct_demand_id(d.demander, d.datapath),
          .bid = d.bid
        };

        demands_table.store((char*)dr.id.get_data(), dr.id.get_size(), (char*)&dr, sizeof(dr));
      }

      static void on(const approve& a) {
        demandrow dr;
        string d_id = construct_demand_id(a.account, a.datapath);
        demands_table.load((char*)d_id.get_data(), d_id.get_size(), (char*)&dr, sizeof(dr));

        historyrow h;
        histories_table.load((char*)a.datapath.get_data(), a.datapath.get_size(), (char*)&h, sizeof(h));

        require_auth(h.owner);

        if (dr.id.get_size() > 0) {
          auto d = demandrow_to_demand(dr);
          require_recipient(d.demander, h.owner);
        } else {
          require_recipient(h.owner);
        }

        h.readers.push_back(a.account);
      }

      static void apply(account_name c, action_name act) {
        eosio::dispatch<contract, sethistory, demand, approve>(c,act);
      }
  };
}
