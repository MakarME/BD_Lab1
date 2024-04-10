#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

class Order {
public:
    static int nextId;
    int id;
    int clientId;
    string car;
    double price;
    double prepayment;
    bool state;

    Order(int client, const string& carName, double orderPrice, double orderPrepayment, bool orderState)
        : id(nextId++), clientId(client), car(carName), price(orderPrice), prepayment(orderPrepayment), state(orderState) {}

    Order(int id_, int client, const string& carName, double orderPrice, double orderPrepayment, bool orderState)
        : id(id_), clientId(client), car(carName), price(orderPrice), prepayment(orderPrepayment), state(orderState) {}

    int getId() const {
        return id;
    }

    int getClientId() const {
        return clientId;
    }

    string getCar() const {
        return car;
    }

    double getPrice() const {
        return price;
    }

    double getPrepayment() const {
        return prepayment;
    }

    bool getState() const {
        return state;
    }

    void setState(bool newState) {
        state = newState;
    }

    void print() const {
        cout << "Order ID: " << id << endl;
        cout << "Client ID: " << clientId << endl;
        cout << "Car: " << car << endl;
        cout << "Price: " << price << endl;
        cout << "Prepayment: " << prepayment << endl;
        cout << "State: " << (state ? "Completed" : "Pending") << endl;
    }
};

int Order::nextId = 0;

class Client {
public:
    static int nextId;
    int id;
    string name;
    string phoneNumber;
    vector<Order> orders;

    Client(const string& clientName, const string& phone)
        : id(nextId++), name(clientName), phoneNumber(phone) {}

    Client(int id_, const string& clientName, const string& phone)
        : id(id_), name(clientName), phoneNumber(phone) {}

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    string getPhoneNumber() const {
        return phoneNumber;
    }

    void addOrder(const Order& order) {
        orders.push_back(order);
    }

    const vector<Order>& getOrders() const {
        return orders;
    }

    void print() const {
        cout << "Client ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phoneNumber << endl;
        cout << "Orders:" << endl;
        for (const auto& order : orders) {
            order.print();
            cout << endl;
        }
    }
};

int Client::nextId = 0;

vector<Client> readClientsFromFile(const string& clientFilename) {
    vector<Client> clients;
    ifstream clientFile(clientFilename);
    ifstream orderFile("orders.txt");

    int maxClientId = 0;
    int maxOrderId = 0;

    if (clientFile.is_open() && orderFile.is_open()) {
        string line;
        while (getline(clientFile, line)) {
            stringstream ss(line);
            string token;
            getline(ss, token, ';');
            int clientId = stoi(token);
            maxClientId = max(maxClientId, clientId);
            getline(ss, token, ';');
            string clientName = token;
            getline(ss, token, ';');
            string phoneNumber = token;

            Client client(clientId, clientName, phoneNumber);

            clients.push_back(client);
        }

        while (getline(orderFile, line)) {
            stringstream ss(line);
            string token;
            getline(ss, token, ';');
            int orderId = stoi(token);
            maxOrderId = max(maxOrderId, orderId);
            getline(ss, token, ';');
            string car = token;
            getline(ss, token, ';');
            int clientId = stoi(token);
            getline(ss, token, ';');
            double price = stod(token);
            getline(ss, token, ';');
            double prepayment = stod(token);
            getline(ss, token, ';');
            bool state = stoi(token);

            for (auto& client : clients) {
                if (client.getId() == clientId) {
                    Order order(orderId, clientId, car, price, prepayment, state);
                    client.addOrder(order);
                    break;
                }
            }
        }

        clientFile.close();
        orderFile.close();

        Client::nextId = maxClientId + 1;
        Order::nextId = maxOrderId + 1;
    }
    else {
        cerr << "Error: Unable to open file for reading" << endl;
    }

    return clients;
}

void writeClientsToFile(const vector<Client>& clients, const string& clientFilename) {
    ofstream clientFile(clientFilename);
    ofstream orderFile("orders.txt");

    if (clientFile.is_open() && orderFile.is_open()) {
        for (const auto& client : clients) {
            clientFile << client.getId() << ";" << client.getName() << ";" << client.getPhoneNumber() << endl;

            const vector<Order>& orders = client.getOrders();
            for (const auto& order : orders) {
                orderFile << order.getId() << ";" << order.getCar() << ";" << order.getClientId()
                    << ";" << order.getPrice() << ";" << order.getPrepayment() << ";" << order.getState() << endl;
            }
        }
        clientFile.close();
        orderFile.close();
    }
    else {
        cerr << "Error: Unable to open file for writing" << endl;
    }
}

void deleteClientByIndex(vector<Client>& clients, int index) {
    int foundIndex = -1;
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].getId() == index) {
            foundIndex = i;
            break;
        }
    }
    if (foundIndex >= 0 && foundIndex < clients.size()) {
        clients.erase(clients.begin() + foundIndex);
    }
    else {
        cerr << "Error: Invalid id" << endl;
    }
}

void deleteOrderByIndex(vector<Client>& clients, int clientId, int orderId) {
    int foundOrderIndex = -1;
    int foundClientIndex = -1;
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].getId() == clientId) {
            foundClientIndex = i;
            for (size_t a = 0; a < clients.size(); ++a) {
                if (clients[i].orders.size() > 0 && clients[i].orders[a].getId() == orderId) {
                    foundOrderIndex = a;
                    break;
                }
            }
            break;
        }
    }
    if (foundClientIndex >= 0 && foundClientIndex < clients.size()) {
        vector<Order>& orders = clients[foundClientIndex].orders;
        if (foundOrderIndex >= 0 && foundOrderIndex < orders.size()) {
            orders.erase(orders.begin() + foundOrderIndex);
        }
        else {
            cerr << "Error: Invalid order id" << endl;
        }
    }
    else {
        cerr << "Error: Invalid client id" << endl;
    }
}

void updateClientField(Client& client, const string& field, const string& value) {
    if (field == "name") {
        client.name = value;
    }
    else if (field == "phone") {
        client.phoneNumber = value;
    }
    else {
        cerr << "Error: Invalid field name" << endl;
    }
}

Order updateOrderField(Order order, const string& field, const string& value) {
    if (field == "car") {
        order.car = value;
    }
    else if (field == "price") {
        order.price = stod(value);
    }
    else if (field == "prepayment") {
        order.prepayment = stod(value);
    }
    else if (field == "state") {
        order.state = (value == "true" || value == "1");
    }
    else {
        cerr << "Error: Invalid field name" << endl;
    }
    return order;
}

void countRecords(const vector<Client>& clients) {
    int totalClients = clients.size();
    int totalOrders = 0;
    for (const auto& client : clients) {
        totalOrders += client.getOrders().size();
    }
    cout << "Total clients: " << totalClients << endl;
    cout << "Total orders: " << totalOrders << endl;
}

void printFileContents(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
    else {
        cerr << "Error: Unable to open file for reading" << endl;
    }
}

int main() {
    vector<Client> clients = readClientsFromFile("clients.txt");

    int choice;
    do {
        cout << "Menu:" << endl;
        cout << "1. Read specified master (get-m)" << endl;
        cout << "2. Read specified slave (get-s)" << endl;
        cout << "3. Delete specified master (del-m)" << endl;
        cout << "4. Delete specified slave (del-s)" << endl;
        cout << "5. Update specified master (update-m)" << endl;
        cout << "6. Update specified slave (update-s)" << endl;
        cout << "7. Insert master (insert-m)" << endl;
        cout << "8. Insert slave (insert-s)" << endl;
        cout << "9. Count master (calc-m)" << endl;
        cout << "10. Count slave (calc-s)" << endl;
        cout << "11. Utilities (ut-m)" << endl;
        cout << "12. Utilities (ut-s)" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cout << "========================================" << endl;

        switch (choice) {
        case 1: {
            int index;
            cout << "Enter id of the master: ";
            cin >> index;
            int foundIndex = -1;
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i].getId() == index) {
                    foundIndex = i;
                    break;
                }
            }
            if (foundIndex >= 0 && foundIndex < clients.size()) {
                cout << "Record at id " << index << ":" << endl;
                clients[foundIndex].print();
                writeClientsToFile(clients, "clients.txt");
            }
            else {
                cerr << "Error: Invalid id" << endl;
            }
            break;
        }
        case 2: {
            int clientId, orderId;
            cout << "Enter id of the master: ";
            cin >> clientId;
            cout << "Enter id of the slave: ";
            cin >> orderId;
            int foundOrderIndex = -1;
            int foundClientIndex = -1;
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i].getId() == clientId) {
                    foundClientIndex = i;
                    for (size_t a = 0; a < clients.size(); ++a) {
                        if (clients[i].orders.size() > 0 && clients[i].orders[a].getId() == orderId) {
                            foundOrderIndex = a;
                            break;
                        }
                    }
                    break;
                }
            }
            if (foundClientIndex >= 0 && foundClientIndex < clients.size()) {
                const vector<Order>& orders = clients[foundClientIndex].getOrders();
                if (foundOrderIndex >= 0 && foundOrderIndex < orders.size()) {
                    cout << "Slave at id " << orderId << ":" << endl;
                    orders[foundOrderIndex].print();
                    writeClientsToFile(clients, "clients.txt");
                }
                else {
                    cerr << "Error: Invalid slave id" << endl;
                }
            }
            else {
                cerr << "Error: Invalid master id" << endl;
            }
            break;
        }
        case 3: {
            int index;
            cout << "Enter id of the master to delete: ";
            cin >> index;
            deleteClientByIndex(clients, index);
            writeClientsToFile(clients, "clients.txt");
            break;
        }
        case 4: {
            int clientIndex, orderIndex;
            cout << "Enter id of the master: ";
            cin >> clientIndex;
            cout << "Enter id of the slave to delete: ";
            cin >> orderIndex;
            deleteOrderByIndex(clients, clientIndex, orderIndex);
            writeClientsToFile(clients, "clients.txt");
            break;
        }
        case 5: {
            int index;
            string field, value;
            cout << "Enter id of the master to update: ";
            cin >> index;
            int foundIndex = -1;
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i].getId() == index) {
                    foundIndex = i;
                    break;
                }
            }
            if (foundIndex >= 0 && foundIndex < clients.size()) {
                cout << "Enter field to update (name/phone): ";
                cin >> field;
                if (field == "name" || field == "phone") {
                    cout << "Enter new value: ";
                    cin >> value;
                    updateClientField(clients[foundIndex], field, value);
                    writeClientsToFile(clients, "clients.txt");
                }
                else {
                    cerr << "Error: Invalid field name" << endl;
                }
            }
            else {
                cerr << "Error: Invalid master id" << endl;
            }
            break;
        }
        case 6: {
            int clientId, orderId;
            string field, value;
            cout << "Enter id of the master: ";
            cin >> clientId;
            int foundClientIndex = -1;
            int foundOrderIndex = -1;
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i].getId() == clientId) {
                    foundClientIndex = i;
                    break;
                }
            }
            if (foundClientIndex >= 0 && foundClientIndex < clients.size()) {
                cout << "Enter id of the slave to update: ";
                cin >> orderId;
                const vector<Order>& orders = clients[foundClientIndex].getOrders();
                for (size_t a = 0; a < orders.size(); ++a) {
                    if (orders.size() > 0 && orders[a].getId() == orderId) {
                        foundOrderIndex = a;
                        break;
                    }
                }
                if (foundOrderIndex >= 0 && foundOrderIndex < orders.size()) {
                    cout << "Enter field to update (car/price/prepayment/state): ";
                    cin >> field;
                    if (field == "car" || field == "price" || field == "prepayment" || field == "state") {
                        cout << "Enter new value: ";
                        cin >> value;
                        clients[foundClientIndex].orders[foundOrderIndex] = updateOrderField(orders[foundOrderIndex], field, value);
                        writeClientsToFile(clients, "clients.txt");
                    }
                    else {
                        cerr << "Error: Invalid field name" << endl;
                    }
                }
                else {
                    cerr << "Error: Invalid slave id" << endl;
                }
            }
            else {
                cerr << "Error: Invalid master id" << endl;
            }
            break;
        }
        case 7: {
            string name, phone;
            cout << "Enter name: ";
            cin >> name;
            cout << "Enter phone: ";
            cin >> phone;
            clients.emplace_back(name, phone);
            writeClientsToFile(clients, "clients.txt");
            break;
        }
        case 8: {
            int clientIndex;
            string car;
            double price, prepayment;
            bool state;
            cout << "Enter master id to add slave: ";
            cin >> clientIndex;
            cout << "Enter car: ";
            cin >> car;
            cout << "Enter price: ";
            cin >> price;
            cout << "Enter prepayment: ";
            cin >> prepayment;
            cout << "Enter state (0/1): ";
            cin >> state;
            int foundIndex = -1;
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i].getId() == clientIndex) {
                    foundIndex = i;
                    break;
                }
            }
            clients[foundIndex].addOrder(Order(clients[foundIndex].getId(), car, price, prepayment, state));
            writeClientsToFile(clients, "clients.txt");
            break;
        }
        case 9: {
            countRecords(clients);
            writeClientsToFile(clients, "clients.txt");
            break;
        }
        case 10: {
            int clientIndex;
            cout << "Enter id of the master to count slaves: ";
            cin >> clientIndex;
            int foundIndex = -1;
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i].getId() == clientIndex) {
                    foundIndex = i;
                    break;
                }
            }
            if (foundIndex >= 0) {
                cout << "Number of slaves for master " << clientIndex << ": " << clients[foundIndex].getOrders().size() << endl;
                writeClientsToFile(clients, "clients.txt");
            }
            else {
                cerr << "Error: Invalid master id" << endl;
            }
            break;
        }
        case 11: {
            printFileContents("clients.txt");
            writeClientsToFile(clients, "clients.txt");
            break;
        }
        case 12: {
            printFileContents("orders.txt");
            writeClientsToFile(clients, "clients.txt");
            break;
        }
        case 0:
            cout << "Exiting..." << endl;
            writeClientsToFile(clients, "clients.txt");
            break;
        default:
            cerr << "Invalid choice. Enter a number between 0 and 12" << endl;
            writeClientsToFile(clients, "clients.txt");
        }
    } while (choice != 0);

    writeClientsToFile(clients, "clients.txt");

    return 0;
}