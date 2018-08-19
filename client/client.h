#pragma once

#include <QtWidgets>

#include "NetworkKeyValueProvider.h"

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);

	void initUi();

	void populateHostCombo();

	~Client();

private slots:
    void onConnect();
	void onGet();
	void onPut();
	void onDelete();

protected:
	void log(const QString& str);

private:
	QLineEdit* _keyStr = nullptr;
	QLineEdit* _valStr = nullptr;
	QTextEdit* _log = nullptr;
    QComboBox *hostCombo = nullptr;
    QLineEdit *portLineEdit = nullptr;
	QPushButton *_connect = nullptr;
	QPushButton* _getBtn = nullptr;
	QPushButton* _putBtn = nullptr;
	QPushButton* _delBtn = nullptr;
	NetworkKeyValueProvider* _provider = nullptr;
};
