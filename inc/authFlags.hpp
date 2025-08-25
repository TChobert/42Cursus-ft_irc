#pragma once

struct authProcessFlags {

	bool _passValidated;
	bool _nickNameSet;
	bool _userNameSet;

	authProcessFlags(void);
	~authProcessFlags(void);
	bool isAuthProcessComplete(void) const;
};
