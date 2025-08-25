#include "authFlags.hpp"

authProcessFlags::authProcessFlags(void) : _passValidated(false), _nickNameSet(false), _userNameSet(false) {}

authProcessFlags::~authProcessFlags(void) {}

bool authProcessFlags::isAuthProcessComplete(void) const {
	return (_passValidated && _nickNameSet && _userNameSet);
}
