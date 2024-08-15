#ifndef SRC_UI_CORE_FINDER_DIALOG_HPP_
#define SRC_UI_CORE_FINDER_DIALOG_HPP_

#include <QProgressDialog>
#include <optional>
#include <memory>
#include "src/core/settings.hpp"
#include "src/core/updates.hpp"

class AbstractCoreFinderDialog : public QProgressDialog {

	protected:
	std::shared_ptr<bool> m_dialogOpen;

	public:
	AbstractCoreFinderDialog();
	~AbstractCoreFinderDialog() {}

	protected:
	virtual void closeEvent( QCloseEvent *event ) override;

};

#ifndef __APPLE__
class MupenCoreFinderDialog : public AbstractCoreFinderDialog {

	private:
	std::optional<CoreBuild> m_build;

	public:
	MupenCoreFinderDialog() : AbstractCoreFinderDialog() {}
	~MupenCoreFinderDialog() {}

	inline const std::optional<CoreBuild> &coreBuild() const noexcept {
		return m_build;
	}

	protected:
	virtual void showEvent( QShowEvent *event ) override;

};
#endif

class ParallelCoreFinderDialog : public AbstractCoreFinderDialog {

	private:
	std::optional<ParallelCoreVersion> m_build;

	public:
	ParallelCoreFinderDialog() : AbstractCoreFinderDialog() {}
	~ParallelCoreFinderDialog() {}

	inline const std::optional<ParallelCoreVersion> &coreBuild() const noexcept {
		return m_build;
	}

	protected:
	virtual void showEvent( QShowEvent *event ) override;

};



#endif /* SRC_UI_CORE_FINDER_DIALOG_HPP_ */
