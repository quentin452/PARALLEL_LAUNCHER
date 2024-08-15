#ifndef RHDC_API_HOST
#define RHDC_API_HOST "https://api.romhacking.com"
#endif

#ifndef SRC_RHDC_WEB_API_HELPERS_HPP_
#define SRC_RHDC_WEB_API_HELPERS_HPP_

#include <functional>
#include <sstream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include "src/rhdc/web/api.hpp"
#include "src/core/json.hpp"

namespace ApiUtil {
	typedef const QByteArray &(*HttpMethodFactory)(void);
}

namespace HttpMethod {
	extern const ApiUtil::HttpMethodFactory Get;
	extern const ApiUtil::HttpMethodFactory Post;
	extern const ApiUtil::HttpMethodFactory Put;
	extern const ApiUtil::HttpMethodFactory Patch;
	extern const ApiUtil::HttpMethodFactory Delete;
	extern const ApiUtil::HttpMethodFactory Head;
}

namespace ApiUtil {

	struct __Identity {
		string username;
		string userId;
		QByteArray authToken;
	};

	extern QNetworkAccessManager *web();
	extern __Identity &identity() noexcept;

	extern bool isOnline();

	extern string urlEncode( const string &str );

	QNetworkRequest makeRequest( const string &url, const char *contentType, bool tryAuthorize );
	inline QNetworkRequest makeRequest( const string &url, bool sendsJson ) {
		return makeRequest( url, sendsJson ? "application/json" : nullptr, true );
	}

	extern int getStatusCode( QNetworkReply *response );
	extern Json readJson( QNetworkReply *response ) noexcept;
	extern void onResponse(
		QNetworkReply *response,
		const std::function<void(void)> &onSuccess,
		const std::function<void(ApiErrorType)> &onFailure
	);

	extern void logResponseError( QNetworkReply *response );

	inline QNetworkReply *send( HttpMethodFactory verb, const string &url ) {
		return web()->sendCustomRequest(
			makeRequest( url, false ),
			verb(),
			QByteArray()
		);
	}

	template<typename T> inline QNetworkReply *send(
		HttpMethodFactory verb,
		const string &url,
		const T &body,
		void(*serializer)(JsonWriter&, const T &obj) = JsonSerializer::serialize<T>
	) {
		std::ostringstream stream;
		JsonWriter jw( &stream, false );
		serializer( jw, body );
		return web()->sendCustomRequest(
			makeRequest( url, true ),
			verb(),
			QByteArray( stream.str().c_str() )
		);
	}

	inline QNetworkReply *send(
		HttpMethodFactory verb,
		const string &url,
		const char *contentType,
		const QByteArray &payload,
		bool tryAuthorize
	) {
		return web()->sendCustomRequest(
			makeRequest( url, contentType, tryAuthorize ),
			verb(),
			payload
		);
	}

}

#endif /* SRC_RHDC_WEB_API_HELPERS_HPP_ */
