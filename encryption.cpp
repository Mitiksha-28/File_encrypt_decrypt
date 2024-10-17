#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

using namespace std;
class EncryptionHandler{
public:
    virtual string encrypt(const string &data, const string &key) = 0;
    virtual string decrypt(const string &data, const string &key) = 0;
    virtual ~EncryptionHandler() = default;
};

class XOREncryption : public EncryptionHandler{
public:
    string encrypt(const string &data, const string &key) override{
        return applyXOR(data, key);
    }

    string decrypt(const string &data, const string &key) override{
        return applyXOR(data, key);
    }

private:
    string applyXOR(const string &data, const string &key){
        string result = data;
        for (size_t i = 0; i < data.size(); ++i){
            result[i] = data[i] ^ key[i % key.size()];
        }
        return result;
    }
};

class CaesarCipher : public EncryptionHandler{
public:
    string encrypt(const string &data, const string &key) override{
        return applyCaesar(data, stoi(key), true);
    }

    string decrypt(const string &data, const string &key) override{
        return applyCaesar(data, stoi(key), false);
    }

private:
    string applyCaesar(const string &data, int shift, bool encrypting){
        string result = data;
        shift = encrypting ? shift : -shift;

        for (char &c : result){
            if (isalpha(c)){
                char base = islower(c) ? 'a' : 'A';
                c = (c - base + shift + 26) % 26 + base;
            }
        }
        return result;
    }
};

class FileHandler{
public:
    string readFile(const string &filename){
        ifstream file(filename, ios::binary);
        if (!file){
            throw runtime_error("Unable to open file: " + filename);
        }

        return string((istreambuf_iterator<char>(file)),
                      istreambuf_iterator<char>());
    }

    void writeFile(const string &filename, const string &data){
        ofstream file(filename, ios::binary);
        if (!file){
            throw runtime_error("Unable to create file: " + filename);
        }
        file << data;
    }
};

int main()
{
    try{
        string password = "password123";
        string inputPassword;

        cout << "Enter password: ";
        getline(cin, inputPassword);

        if (inputPassword != password)
        {
            cout << "Authentication failed!\n";
            return 1;
        }

        FileHandler fileHandler;
        string filename, key;

        cout << "Enter input filename: ";
        getline(cin, filename);

        cout << "Select encryption method:\n"
             << "1. XOR Encryption\n"
             << "2. Caesar Cipher\n"
             << "Choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        shared_ptr<EncryptionHandler> encryptor;
        switch (choice)
        {
        case 1:
            encryptor = make_shared<XOREncryption>();
            break;
        case 2:
            encryptor = make_shared<CaesarCipher>();
            break;
        default:
            throw runtime_error("Invalid encryption method selected");
        }

        cout << "Enter encryption key: ";
        getline(cin, key);

        cout << "Choose operation:\n"
             << "1. Encrypt\n"
             << "2. Decrypt\n"
             << "Choice: ";

        int operation;
        cin >> operation;

        string data = fileHandler.readFile(filename);
        string output;
        string outputFilename;

        if (operation == 1)
        {
            output = encryptor->encrypt(data, key);
            outputFilename = "encrypted_" + filename;
        }
        else if (operation == 2)
        {
            output = encryptor->decrypt(data, key);
            outputFilename = "decrypted_" + filename;
        }
        else
        {
            throw runtime_error("Invalid operation selected");
        }

        fileHandler.writeFile(outputFilename, output);
        cout << "Operation completed successfully. Output saved to: " << outputFilename << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}