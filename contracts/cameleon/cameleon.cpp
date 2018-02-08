/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "cameleon.hpp"

namespace cameleon {
  void apply_cameleon_transfer(const cameleon::transfer& transfer_storage) {
    eosio::require_recipient( transfer_storage.to, transfer_storage.from );
    eosio::require_auth( transfer_storage.from );

    account from = get_account( transfer_storage.from );
    account to   = get_account( transfer_storage.to );

    from.balance -= transfer_storage.quantity; /// token subtraction has underflow assertion
    to.balance   += transfer_storage.quantity; /// token addition has overflow assertion

    store_account( transfer_storage.from, from );
    store_account( transfer_storage.to, to );
  }

  void apply_cameleon_sethistory(const history& history_to_set) {

  }

  void apply_cameleon_removehistory(const char *data_path) {

  }

  void demand_to_id(char *id, const demand& demand_to_convert) {

  }

  void apply_cameleon_demand(const demand& demand_to_post) {

  }

  void apply_cameleon_approve(const approve& approve_target) {

  }
}

using namespace cameleon;

/**
 *  The init() and apply() methods must have C calling convention so that the blockchain can lookup and
 *  call these methods.
 */
extern "C" {

  /// The apply method implements the dispatch of events to this contract
  void apply( uint64_t code, uint64_t action ) {
    eosio::print( "Hello World: ", eosio::name(code), "->", eosio::name(action), "\n" );
    if(code == N(cameleon)) {
      if(action == N(transfer)) {
        apply_cameleon_transfer( eosio::current_actions<cameleon::transfer>() );
      } else if (action == N(sethistory)) {
        char tmp[1025];
        auto len = read_action( tmp, 1025 );
        apply_cameleon_sethistory( tmp, len );
      } else if (action == N(removehistory)) {
        char tmp[1025];
        auto len = read_action( tmp, 1025 );
        apply_cameleon_removehistory( tmp, len );
      } else if (action == N(demand)) {
        char tmp[1025];
        auto len = read_action( tmp, 1025 );
        apply_cameleon_demand( tmp, len );
      } else if (action == N(approve)) {
        char tmp[1025];
        auto len = read_action( tmp, 1025 );
        apply_cameleon_approve( tmp, len );
      } else {
        assert(0, "unknown message");
      }
    } else {
      assert(0, "unknown code");
    }
  }

} // extern "C"
