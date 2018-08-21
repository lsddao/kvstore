#include "client.h"

#include <QtNetwork>

Client::Client(QWidget *parent)
    : QDialog(parent)
    , hostCombo(new QComboBox)
    , portLineEdit(new QLineEdit)
    , _connect(new QPushButton("Connect"))
	, _getBtn(new QPushButton("Get"))
	, _putBtn(new QPushButton("Put"))
	, _delBtn(new QPushButton("Delete"))
	, _keyStr(new QLineEdit)
	, _valStr(new QLineEdit)
	, _log(new QTextEdit)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	initUi();
	populateHostCombo();
}

void Client::initUi()
{
	hostCombo->setEditable(true);

	portLineEdit->setValidator(new QIntValidator(1, 65535, this));

	auto hostLabel = new QLabel("Server name:");
	hostLabel->setBuddy(hostCombo);
	auto portLabel = new QLabel("Server port:");
	portLabel->setBuddy(portLineEdit);

	auto buttonBox = new QDialogButtonBox;

	buttonBox->addButton(_connect, QDialogButtonBox::ActionRole);
	buttonBox->addButton(_getBtn, QDialogButtonBox::ActionRole);
	buttonBox->addButton(_putBtn, QDialogButtonBox::ActionRole);
	buttonBox->addButton(_delBtn, QDialogButtonBox::ActionRole);

	connect(_connect, &QAbstractButton::clicked, this, &Client::onConnect);
	connect(_getBtn, &QAbstractButton::clicked, this, &Client::onGet);
	connect(_putBtn, &QAbstractButton::clicked, this, &Client::onPut);
	connect(_delBtn, &QAbstractButton::clicked, this, &Client::onDelete);

	_keyStr->setPlaceholderText("key");
	_valStr->setPlaceholderText("value");
	_log->setReadOnly(true);

	QGridLayout *mainLayout = new QGridLayout(this);

	mainLayout->addWidget(hostLabel, 0, 0);
	mainLayout->addWidget(hostCombo, 0, 1);
	mainLayout->addWidget(portLabel, 1, 0);
	mainLayout->addWidget(portLineEdit, 1, 1);
	mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
	mainLayout->addWidget(_keyStr, 4, 0, 1, 2);
	mainLayout->addWidget(_valStr, 5, 0, 1, 2);
	mainLayout->addWidget(_log, 6, 0, 4, 2);

	setWindowTitle(QGuiApplication::applicationDisplayName());
	portLineEdit->setFocus();
}

void Client::populateHostCombo()
{
	// find out name of this machine
	QString name = QHostInfo::localHostName();
	if (!name.isEmpty()) {
		hostCombo->addItem(name);
		QString domain = QHostInfo::localDomainName();
		if (!domain.isEmpty())
			hostCombo->addItem(name + QChar('.') + domain);
	}
	if (name != QLatin1String("localhost"))
		hostCombo->addItem(QString("localhost"));
	// find out IP addresses of this machine
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// add non-localhost addresses
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (!ipAddressesList.at(i).isLoopback())
			hostCombo->addItem(ipAddressesList.at(i).toString());
	}
	// add localhost addresses
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i).isLoopback())
			hostCombo->addItem(ipAddressesList.at(i).toString());
	}
}

Client::~Client()
{
	if (_provider)
		delete _provider;
}

void Client::onConnect()
{
	try
	{
		_provider = new NetworkKeyValueProvider(hostCombo->currentText(), portLineEdit->text().toUShort());
		log("connected");
		_connect->setDisabled(true);
	}
	catch (const std::exception& ex)
	{
		QMessageBox::critical(this, "Error", ex.what());
	}
}

void Client::onGet()
{
	try
	{
		_valStr->setText(_provider->value(_keyStr->text()));
		logResponse();
	}
	catch (const std::exception& ex)
	{
		log(ex.what());
	}
}

void Client::logResponse()
{
	QStringList lstLog = QStringList() << _provider->lastResponseError();
	if (!_provider->lastResponseDetails().isEmpty())
		lstLog << _provider->lastResponseDetails();
	log(lstLog.join(": "));
}

void Client::onPut()
{
	try
	{
		_provider->insert(_keyStr->text(), _valStr->text());
		logResponse();
	}
	catch (const std::exception& ex)
	{
		log(ex.what());
	}
}

void Client::onDelete()
{
	try
	{
		_valStr->clear();
		_provider->remove(_keyStr->text());
		logResponse();
	}
	catch (const std::exception& ex)
	{
		log(ex.what());
	}
}

void Client::log(const QString& str)
{
	_log->append(str);
}