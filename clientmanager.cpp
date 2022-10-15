#include "clientmanager.h"
#include "ui_clientmanager.h"
#include "client.h"

#include <QFile>
#include <QMenu>

ClientManager::ClientManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManager)
{
    ui->setupUi(this);
    ui->toolBox->setCurrentIndex(0);                                    /*toolbox의 시작 위치는 0번째 부터(Input인자)*/

    QAction* removeAction = new QAction(tr("&Remove"));                 /*제거 액션 할당*/
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));     /*removeAction 연결*/

    menu = new QMenu;                                                   /*메뉴 할당*/
    menu->addAction(removeAction);                                      /*메뉴에 removeAction 추가*/
    ui->ClientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);  /*contextMenuEvent() 처리기가 호출됨을 의미*/
    connect(ui->ClientTreeWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));

    /*데이터 보내기*/
    connect(ui->CNameLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(ClientAdded(QString)));


    /*파일 불러오기*/
    QFile file("clientlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            Client* c = new Client(id, row[1], row[2], row[3]);
            ui->ClientTreeWidget->addTopLevelItem(c);
            clientList.insert(id, c);

            emit ClientAdded(row[1]);
        }
    }
    file.close( );
}

ClientManager::~ClientManager()
{
    delete ui;

    /*파일 저장*/
    QFile file("clientlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : clientList) {
        Client* c = v;
        out << c->id() << ", " << c->getName() << ", ";
        out << c->getPhoneNumber() << ", ";
        out << c->getAddress() << "\n";
    }
    file.close( );
}

//QString ClientManager::getClientName()
//{
//    return ui->CNameLineEdit->text();
//}

int ClientManager::makeID( )
{
    if(clientList.size( ) == 0) {
        return 1;
    } else {
        auto id = clientList.lastKey();
        return ++id;
    }
}

void ClientManager::removeItem()
{
    QTreeWidgetItem* item = ui->ClientTreeWidget->currentItem();
    if(item != nullptr) {
        clientList.remove(item->text(0).toInt());
        ui->ClientTreeWidget->takeTopLevelItem
                (ui->ClientTreeWidget->indexOfTopLevelItem(item));
        ui->ClientTreeWidget->update();
    }
}

void ClientManager::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->ClientTreeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}


void ClientManager::on_InputButton_clicked()
{
    QString name, number, address;
    int id = makeID( );
    name = ui->CNameLineEdit->text();
    number = ui->CPhoneLineEdit->text();
    address = ui->CEmailLineEdit->text();
    if(name.length()) {
        Client* c = new Client(id, name, number, address);
        clientList.insert(id, c);
        ui->ClientTreeWidget->addTopLevelItem(c);
        emit ClientAdded(name);
    }
}


void ClientManager::on_CancelButton_clicked()
{
    ui->CIDLineEdit->setText("");
    ui->CNameLineEdit->setText("");
    ui->CPhoneLineEdit->setText("");
    ui->CEmailLineEdit->setText("");
}

void ClientManager::on_ModifyButton_clicked()
{
    QTreeWidgetItem* item = ui->ClientTreeWidget->currentItem();
    if(item != nullptr) {
        int key = item->text(0).toInt();
        Client* c = clientList[key];
        QString name, number, address;
        name = ui->CNameLineEdit->text();
        number = ui->CPhoneLineEdit->text();
        address = ui->CEmailLineEdit->text();
        c->setName(name);
        c->setPhoneNumber(number);
        c->setAddress(address);
        clientList[key] = c;
    }
}


void ClientManager::on_ClientTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->CIDLineEdit->setText(item->text(0));
    ui->CNameLineEdit->setText(item->text(1));
    ui->CPhoneLineEdit->setText(item->text(2));
    ui->CEmailLineEdit->setText(item->text(3));
    ui->toolBox->setCurrentIndex(0);
}


void ClientManager::on_SearchButton_clicked()
{
    ui->ClientSearchTree->clear();
    int i = ui->SearchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->ClientTreeWidget->findItems
                (ui->SearchLineEdit->text(), flag, i);

        foreach(auto i, items) {
            Client* c = static_cast<Client*>(i);
            int id = c->id();
            QString name = c->getName();
            QString number = c->getPhoneNumber();
            QString address = c->getAddress();
            Client* item = new Client(id, name, number, address);
            ui->ClientSearchTree->addTopLevelItem(item);
        }
    }
}


void ClientManager::on_ClientSearchTree_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->CNameLineEdit->setText(item->text(1));
}

