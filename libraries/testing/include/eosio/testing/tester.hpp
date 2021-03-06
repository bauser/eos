#pragma once
#include <eosio/chain/chain_controller.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iostream>

namespace eosio { namespace testing {

   using namespace eosio::chain;


   /**
    *  @class tester
    *  @brief provides utility function to simplify the creation of unit tests
    */
   class tester {
      public:
         typedef string action_result;

         tester(bool process_genesis = true);

         void              close();
         void              open();
         void              create_init_accounts();

         signed_block      produce_block( fc::microseconds skip_time = fc::milliseconds(config::block_interval_ms) );
         void              produce_blocks( uint32_t n = 1 );

         transaction_trace push_transaction( packed_transaction& trx );
         transaction_trace push_transaction( signed_transaction& trx );
         action_result      push_action(action&& cert_act, uint64_t authorizer);
         void              set_tapos( signed_transaction& trx ) const;

         void              create_accounts( vector<account_name> names, asset init_bal, bool multisig = false ) {
            for( auto n : names ) create_account(n, init_bal, N(inita), multisig );
         }


         void set_authority( account_name account, permission_name perm, authority auth,
                                     permission_name parent = config::owner_name );

         void              create_account( account_name name, asset initial_balance = asset(), account_name creator = N(inita), bool multisig = false );
         void              create_account( account_name name, string balance = "0.0000 EOS", account_name creator = N(inita), bool multisig = false );

         transaction_trace push_nonce( account_name from, const string& role, const string& v = "blah" );
         transaction_trace transfer( account_name from, account_name to, asset amount, string memo = "", account_name currency = config::eosio_system_account_name );
         transaction_trace transfer( account_name from, account_name to, string amount, string memo = "", account_name currency = config::eosio_system_account_name );

         template<typename ObjectType, typename IndexBy, typename... Args>
         const auto& get( Args&&... args ) {
            return control->get_database().get<ObjectType,IndexBy>( forward<Args>(args)... );
         }

         public_key_type   get_public_key( name keyname, string role = "owner" ) const;
         private_key_type  get_private_key( name keyname, string role = "owner" ) const;

         void              set_code( account_name name, const char* wast );
         void              set_abi( account_name name, const char* abi_json );


         unique_ptr<chain_controller> control;

         bool                          chain_has_transaction( const transaction_id_type& txid ) const;
         const transaction_receipt&    get_transaction_receipt( const transaction_id_type& txid ) const;

         share_type                    get_balance( const account_name& account ) const;

         asset                         get_currency_balance( const account_name& contract,
                                                             const symbol&       asset_symbol,
                                                             const account_name& account ) const;

        static vector<uint8_t> to_uint8_vector(const string& s);

        static vector<uint8_t> to_uint8_vector(uint64_t x);

        static uint64_t to_uint64(fc::variant x);

        static string to_string(fc::variant x);

        static action_result success() { return string(); }

        static action_result error(const string& msg) { return msg; }

      private:
         fc::temp_directory                            tempdir;
         chain_controller::controller_config           cfg;

         map<transaction_id_type, transaction_receipt> chain_transactions;
   };

   /**
    * Utility predicate to check whether an FC_ASSERT message ends with a given string
    */
   struct assert_message_is {
      assert_message_is(string expected)
         :expected(expected)
      {}

      bool operator()( const fc::assert_exception& ex ) {
         auto message = ex.get_log().at(0).get_message();
         return boost::algorithm::ends_with(message, expected);
      }

      string expected;
   };


} } /// eosio::testing
