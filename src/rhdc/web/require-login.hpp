#ifndef SRC_RHDC_WEB_REQUIRE_LOGIN_HPP_
#define SRC_RHDC_WEB_REQUIRE_LOGIN_HPP_

#include "src/rhdc/core/credentials.hpp"
#include "src/rhdc/web/api-helpers.hpp"

#define REQUIRE_LOGIN( retryFunc ) \
	if( !RhdcApi::isAuthenticated() ) { \
		const RhdcCredentials credentials = RhdcCredentials::load(); \
		if( credentials.legacyPassword.empty() && credentials.jwt.empty() ) { \
			onFailure( ApiErrorType::NotAuthorized ); \
		} else if( !credentials.jwt.empty() ) { \
			ApiUtil::identity().authToken = QByteArray( credentials.jwt.c_str() ); \
			RhdcApi::verifyLoginAsync( retryFunc, onFailure ); \
		} else if( !credentials.username.empty() && !credentials.legacyPassword.empty() ) { \
			RhdcApi::loginAsync( credentials.username, credentials.legacyPassword, -1, false, retryFunc, onFailure ); \
		} else { \
			onFailure( ApiErrorType::NotAuthorized ); \
		} \
	}

#define OPTIONAL_LOGIN( retryFunc ) { \
	static bool __tryAuthorize = true; \
	if( __tryAuthorize && !RhdcApi::isAuthenticated() ) { \
		if( ApiUtil::identity().userId.empty() ) { \
			const RhdcCredentials credentials = RhdcCredentials::load(); \
			if( !credentials.jwt.empty() ) { \
				__tryAuthorize = false; \
				const auto __retry = retryFunc; \
				ApiUtil::identity().authToken = QByteArray( credentials.jwt.c_str() ); \
				RhdcApi::verifyLoginAsync( __retry, [__retry](ApiErrorType){ __retry(); } ); \
				return; \
			} else if( !credentials.legacyPassword.empty() ) { \
				__tryAuthorize = false; \
				const auto __retry = retryFunc; \
				RhdcApi::loginAsync( credentials.username, credentials.legacyPassword, -1, false, __retry, [__retry](ApiErrorType){ __retry(); } ); \
				return; \
			} \
		} \
	} \
	__tryAuthorize = true; \
}



#endif /* SRC_RHDC_WEB_REQUIRE_LOGIN_HPP_ */
