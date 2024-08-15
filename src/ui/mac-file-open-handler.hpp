#ifndef SRC_UI_MAC_FILE_OPEN_HANDLER_HPP_
#define SRC_UI_MAC_FILE_OPEN_HANDLER_HPP_

#include <QFileOpenEvent>
#include <QObject>
#include "src/core/filesystem.hpp"

class MacFileOpenHandler : public QObject {
	Q_OBJECT

	private:
	fs::path *m_path;

	public:
	MacFileOpenHandler( fs::path *pathOutput ) : QObject( nullptr ), m_path( pathOutput ) {}
	~MacFileOpenHandler() {}

	inline void doneStartup() { m_path = nullptr; }

	bool eventFilter( QObject *object, QEvent *event ) override;

	private:
	void handleDuringStartup( const QFileOpenEvent &event ) const;
	void handleAfterStartup( const QFileOpenEvent &event ) const;

};



#endif /* SRC_UI_MAC_FILE_OPEN_HANDLER_HPP_ */
