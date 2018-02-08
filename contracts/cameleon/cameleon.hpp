/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosiolib/eosio.hpp>
#include <eosiolib/token.hpp>
#include <eosiolib/db.hpp>

namespace cameleon {
  typedef eosio::token<uint64_t, N(cameleon)> cameleon_tokens;

  struct transfer {
    account_name from;
    account_name to;
    cameleon_tokens amount;
  };

  struct PACKED(account)
  {
    account( cameleon_tokens b = cameleon_tokens() ):balance(b){}

    /**
     *  The key is constant because there is only one record per scope/currency/accounts
     */
    const uint64_t key = N(account);

    /**
    * Balance number of tokens in account
    **/
    cameleon_tokens balance;

    /**
    Method to check if accoutn is empty.
    @return true if account balance is zero and there is quota used.
    **/
    bool is_empty()const  { return balance.quantity == 0 }
  };

  /**
  Assert statement to verify structure packing for account
  **/
  static_assert( sizeof(account) == sizeof(uint64_t)+sizeof(cameleon_tokens)+sizeof(uint64_t), "unexpected packing" );

  inline account get_account( account_name owner ) {
    account owned_account;
    accounts::get( owned_account, owner );
    return owned_account;
  }

  struct PACKED(history)
  {
    char *datapath;
    account_name owner;
    uint8_t *enckey
    char *readerspath;
  };

  struct PACKED(demand)
  {
    account_name owner;
    char *datapath;
    uint64_t bid;
  };

  struct PACKED(demand_to_store)
  {
    char *id;
    uint64_t bid;
  };

  struct PACKED(approve)
  {
    account_name account;
    char *datapath;
  };

  using accounts = eosio::table<N(cameleon),N(cameleon),N(accounts),account,uint64_t>;
  using histories = eosio::table<N(cameleon),N(cameleon),N(histories),history,char*>;
  using demands = eosio::table<N(cameleon),N(cameleon),N(demands),demand_to_store,char*>;
}
