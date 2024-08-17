#ifndef SRC_UI_FLOW_LAYOUT_HPP_
#define SRC_UI_FLOW_LAYOUT_HPP_

#include <QLayout>
#include <QList>

class FlowLayout final : public QLayout {

private:
  QList<QLayoutItem *> m_items;
  QSize m_spacing;

  int doLayout(const QRect &rect, bool test) const;

public:
  FlowLayout(QWidget *parent = nullptr);
  FlowLayout(QWidget *parent, QSize &&spacing);
  ~FlowLayout();

  QSize getItemSpacing() const;
  void setItemSpacing(QSize &&spacing);

  Qt::Orientations expandingDirections() const override;
  bool hasHeightForWidth() const override;
  int heightForWidth(int width) const override;
  int count() const override;
  QLayoutItem *itemAt(int index) const override;
  QSize minimumSize() const override;
  QSize sizeHint() const override;

  void addItem(QLayoutItem *item) override;
  void setGeometry(const QRect &rect) override;
  QLayoutItem *takeAt(int index) override;
};

#endif /* SRC_UI_FLOW_LAYOUT_HPP_ */
