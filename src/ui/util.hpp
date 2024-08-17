#ifndef SRC_UI_UTIL_HPP_
#define SRC_UI_UTIL_HPP_

#include <QAbstractButton>
#include <QDialog>
#include <QLabel>
#include <QLayout>

namespace UiUtil {

extern void clearLayout(QLayout *layout);
extern void shrinkToFit(QDialog *dialog,
                        const QFlags<Qt::Orientation> &shrinkDirection);
extern void setIconSize(QAbstractButton *button, int baseIconSize);
extern void scaleWindow(QWidget *window);

#ifdef __APPLE__
extern void fixFontSizeOnMac(QLabel *label);
#else
inline void fixFontSizeOnMac(QLabel *) {}
#endif

#ifdef _WIN32
extern void fixDialogButtonsOnWindows(QDialog *dialog);
#else
inline void fixDialogButtonsOnWindows(QDialog *) {}
#endif

} // namespace UiUtil

#endif /* SRC_UI_UTIL_HPP_ */
