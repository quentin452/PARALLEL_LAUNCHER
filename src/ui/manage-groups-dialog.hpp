#ifndef SRC_UI_MANAGE_GROUPS_DIALOG_HPP_
#define SRC_UI_MANAGE_GROUPS_DIALOG_HPP_

#include <set>
#include <QDialog>
#include "src/types.hpp"

namespace Ui {
	class ManageGroupsDialog;
}

struct _MGD_GroupInfo {
	string name;
	int romCount;
};
Q_DECLARE_METATYPE( _MGD_GroupInfo )


class ManageGroupsDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::ManageGroupsDialog *m_ui;
	std::set<string> m_groups;

	public:
	explicit ManageGroupsDialog( QWidget *parent = nullptr );
	virtual ~ManageGroupsDialog();

	private slots:
	void updateButtons();
	void deleteGroup();
	void renameGroup();
	void addGroup();

	protected:
	virtual void showEvent( QShowEvent *event ) override;

};



#endif /* SRC_UI_MANAGE_GROUPS_DIALOG_HPP_ */
