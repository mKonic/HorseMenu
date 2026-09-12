#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/IntCommand.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Features
{
	// Story cash goes through the game's own money natives, no globals needed.
	// The reason hash is the one the game's money pile scripts pass, and the
	// balance is in cents, so a dollar is 100.
	constexpr Hash MoneyAddReason = 752097756;

	static IntCommand _MoneyAmount{"moneyamount", "Amount ($)", "How many dollars to add", 1, 100000, 100};

	class AddMoney : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const auto dollars = Commands::GetCommand<IntCommand>("moneyamount"_J)->GetState();
			const auto before  = MONEY::_MONEY_GET_CASH_BALANCE();

			MONEY::_MONEY_INCREMENT_CASH_BALANCE(dollars * 100, MoneyAddReason);

			LOG(INFO) << "Added $" << dollars << ": balance went from " << before << " to " << MONEY::_MONEY_GET_CASH_BALANCE() << " (cents)";
		}
	};

	static AddMoney _AddMoney{"addmoney", "Add Money", "Adds the amount above to your cash"};
}
