#ifndef SRC_DB_TRANSACTION_HPP_
#define SRC_DB_TRANSACTION_HPP_

class SqlTransaction final {

	private:
	bool m_committed;

	public:
	SqlTransaction() noexcept;
	SqlTransaction( SqlTransaction &&other ) noexcept;
	SqlTransaction( const SqlTransaction &other ) = delete;
	~SqlTransaction() noexcept;

	void commit() noexcept;

};

#endif /* SRC_DB_TRANSACTION_HPP_ */
