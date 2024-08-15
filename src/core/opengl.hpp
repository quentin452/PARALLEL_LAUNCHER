#ifndef SRC_CORE_OPENGL_HPP_
#define SRC_CORE_OPENGL_HPP_

#include <initializer_list>
#include <unordered_set>
#include "src/core/version.hpp"

class OpenGlInfo {

	public:
	const string name;
	const string vendor;
	const Version glVersion;
	const string shaderVersion;
	const HashSet<string> extensions;

	private:
	OpenGlInfo(
		string &&_name,
		string &&_vendor,
		const Version &_glVersion,
		string &&_shaderVersion,
		HashSet<string> &&_extensions
	) :
		name( std::move( _name ) ),
		vendor( std::move( _vendor ) ),
		glVersion( _glVersion ),
		shaderVersion( std::move( _shaderVersion ) ),
		extensions( std::move( _extensions ) )
	{}

	public:
	inline ~OpenGlInfo() {}

	void serialize(
		JsonWriter &jw,
		const std::initializer_list<const char*> &relevantExtensions
	) const;

	static std::optional<OpenGlInfo> getForActiveDevice();

};

#endif /* SRC_CORE_OPENGL_HPP_ */
