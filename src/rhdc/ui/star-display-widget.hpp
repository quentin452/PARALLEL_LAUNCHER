#ifndef STARDISPLAYWIDGET_HPP
#define STARDISPLAYWIDGET_HPP

#include "src/core/filesystem.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/rhdc/ui/star-set.hpp"
#include <QFileSystemWatcher>
#include <QTabWidget>
#include <QVBoxLayout>
#include <vector>

struct SDWLayoutSides {
  QVBoxLayout *top;
  QVBoxLayout *right;
  QVBoxLayout *bottom;
  QVBoxLayout *left;
};

class StarDisplayWidget : public QTabWidget {
  Q_OBJECT

protected:
  const StarLayout m_layout;
  std::vector<StarSetUi *> m_starSets;

  StarDisplayWidget(QWidget *parent, StarLayout &&layout);
  virtual ~StarDisplayWidget() {}

  void setup(bool editable);
  virtual SDWLayoutSides makeColumnContainer(QWidget *parent, int slot) = 0;

public:
  inline const StarLayout &starLayout() const noexcept { return m_layout; }
  void makeScrollable();
};

class EditableStarDisplayWidget : public StarDisplayWidget {
  Q_OBJECT

private:
  std::vector<bool> m_slotActive;
  const fs::path m_saveFilePath;

public:
  EditableStarDisplayWidget(QWidget *parent, const fs::path &saveFilePath,
                            StarLayout &&layout);
  virtual ~EditableStarDisplayWidget() {}

  void save() const;
  void load();

protected:
  virtual SDWLayoutSides makeColumnContainer(QWidget *parent,
                                             int slot) override;
};

class LiveStarDisplayWidget : public StarDisplayWidget {
  Q_OBJECT

private:
  QFileSystemWatcher m_fileWatcher;
  const fs::path m_saveFilePath;

public:
  LiveStarDisplayWidget(QWidget *parent, const fs::path &saveFilePath,
                        StarLayout &&layout);
  virtual ~LiveStarDisplayWidget() {}

private slots:
  void saveFileChanged();

protected:
  virtual SDWLayoutSides makeColumnContainer(QWidget *parent,
                                             int slot) override;
};

#endif // STARDISPLAYWIDGET_HPP
