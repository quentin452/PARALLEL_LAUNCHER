#include "src/rhdc/ui/rhdc-save-editor.hpp"

#include "src/core/qthread.hpp"
#include "src/rhdc/ui/star-display-widget.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include <QApplication>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>
#include <memory>

class RhdcSaveEditorDialog : public QDialog {

private:
  std::shared_ptr<bool> m_exists;
  EditableStarDisplayWidget *m_editor;

public:
  RhdcSaveEditorDialog()
      : QDialog(), m_exists(new bool(true)), m_editor(nullptr) {}
  ~RhdcSaveEditorDialog() { *m_exists = false; }

  inline void setEditor(EditableStarDisplayWidget *editor) noexcept {
    m_editor = editor;
  }

protected:
  virtual void setVisible(bool visible) override {
    QDialog::setVisible(visible);
    if (!visible || !*m_exists)
      return;

    std::shared_ptr<bool> isAlive = m_exists;
    QtThread::safeAsync([this, isAlive]() {
      if (!*isAlive || !this->m_editor)
        return;
      this->m_editor->adjustSize();
      this->adjustSize();

      const QRect workspace = QApplication::desktop()->availableGeometry(this);
      const QMargins margins = (this->windowHandle() != nullptr)
                                   ? this->windowHandle()->frameMargins()
                                   : QMargins(0, 0, 0, 0);
      QRect geo = this->geometry();
      bool requiresScrollbar = false;

      const int decorationHeight = margins.top() + margins.bottom();
      if (geo.height() + decorationHeight > workspace.height()) {
        geo.setTop(workspace.top() + margins.top());
        geo.setHeight(workspace.height() - decorationHeight);
        geo.setWidth(geo.width() + 24);
        requiresScrollbar = true;
      }

      const int decorationWidth = margins.left() + margins.right();
      if (geo.width() + decorationWidth > workspace.width()) {
        geo.setLeft(workspace.left() + margins.left());
        geo.setWidth(workspace.width() - decorationWidth);
        requiresScrollbar = true;
      }

      if (!requiresScrollbar)
        return;

      QLayout *rootLayout = this->layout();
      if (!rootLayout)
        return;

      this->m_editor->makeScrollable();
      this->setGeometry(geo);

      QtThread::safeAsync([this, isAlive, geo]() {
        if (!*isAlive)
          return;
        this->setMaximumWidth(geo.width());
        this->setMaximumHeight(geo.height());
        this->setGeometry(geo);
      });
    });
  }
};

void RhdcSaveEditor::exec(const fs::path &saveFilePath, const string &hackId) {
  StarLayout layout;
  if (!StarLayout::tryLoadLayout(hackId, layout)) {
    return;
  }

  RhdcSaveEditorDialog dialog;
  dialog.setModal(true);
  dialog.setSizeGripEnabled(true);
  dialog.setWindowTitle(
      QCoreApplication::translate("RhdcSaveEditor", "Save Editor"));
  dialog.setWindowIcon(Icon::appIcon());
  UiUtil::fixDialogButtonsOnWindows(&dialog);

  QVBoxLayout *widgets = new QVBoxLayout(&dialog);
  dialog.setLayout(widgets);

  EditableStarDisplayWidget *editor =
      new EditableStarDisplayWidget(&dialog, saveFilePath, std::move(layout));
  QDialogButtonBox *buttons = new QDialogButtonBox(
      QDialogButtonBox::Save | QDialogButtonBox::Discard, &dialog);

  QHBoxLayout *editorLayout = new QHBoxLayout();
  editorLayout->addWidget(editor, 0);
  editorLayout->addSpacing(1);
  editorLayout->setSizeConstraint(QLayout::SetNoConstraint);
  dialog.setEditor(editor);

  widgets->addLayout(editorLayout, 1);
  widgets->addWidget(buttons, 0);
  widgets->setSizeConstraint(QLayout::SetMinAndMaxSize);

  QObject::connect(buttons->button(QDialogButtonBox::Discard),
                   &QPushButton::clicked, &dialog, &QDialog::reject);
  QObject::connect(buttons->button(QDialogButtonBox::Save),
                   &QPushButton::clicked, &dialog, [&]() {
                     editor->save();
                     dialog.accept();
                   });

  dialog.exec();
}
