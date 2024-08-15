#ifndef SRC_UI_IS_VIEWER_WINDOW_HPP_
#define SRC_UI_IS_VIEWER_WINDOW_HPP_

#include <QMainWindow>

class IsViewerWindow : public QMainWindow {
	Q_OBJECT

	public:
	IsViewerWindow();
	~IsViewerWindow();

	public slots:
	void logMessage( QString message );
	void error( bool connected );

};

#endif /* SRC_UI_IS_VIEWER_WINDOW_HPP_ */
