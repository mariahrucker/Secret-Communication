#include <iostream>

#include <fstream>

#include <string>

#include <bitset>

#include <opencv2/opencv.hpp>

#include <openssl/sha.h>

#include <openssl/evp.h>

using namespace std;
using namespace cv;

void store_hashed_password(string hashed_password) {
  // Open file for writing
  ofstream outfile;
  outfile.open("hashed_password.txt");

  // Write hashed password to file
  outfile << hashed_password;

  // Close file
  outfile.close();
}

string retrieve_hashed_password() {
  // Open file for reading
  ifstream infile;
  infile.open("hashed_password.txt");

  // Read hashed password from file
  string hashed_password;
  infile >> hashed_password;

  // Close file
  infile.close();

  return hashed_password;
}

void decrypt(string message, string key, string hashed_password_stored, string hashed_password_input) {
  // Check if hashed password is correct
  if (hashed_password_stored != hashed_password_input) {
    cout << "Incorrect password. Access denied." << endl;
    return;
  }

  // Decrypt message
  string decrypted_message;
  int key_length = key.length();
  int message_length = message.length();

  for (int i = 0; i < message_length; i++) {
    int key_index = i % key_length;
    int shift = key[key_index] - 'a';
    char c = message[i] - shift;
    if (c < 'a') {
      c += 26;
    }
    decrypted_message += c;
  }

  cout << "Decrypted message: " << decrypted_message << endl;
}

int main(int argc, char ** argv) {

    // Prompt user for password
    cout << "Please enter the password: ";
    string password;
    cin >> password;

    // Compute hash of user input password
    unsigned char hashed_password_input[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char * ) password.c_str(), password.length(), hashed_password_input);
    string hashed_password_input_str((const char * ) hashed_password_input, SHA256_DIGEST_LENGTH);

    // Read in hashed password stored in a file
    string hashed_password_stored = retrieve_hashed_password();
    if (hashed_password_stored.empty()) {
      cout << "Error: Unable to open hashed_password.txt file." << endl;
      return -1;
    }

    // Check if user input password matches stored password
    if (hashed_password_stored != hashed_password_input_str) {
      cout << "Incorrect password. Access denied." << endl;
      return -1;
    }

    // Read input image file
    Mat image = imread("input.png", IMREAD_COLOR);

    // Get image size
    int height = image.rows;
    int width = image.cols;

    // Convert image to grayscale
    cvtColor(image, image, COLOR_BGR2GRAY);

    // Read secret message from file
    string message;
    ifstream message_file("secret_message.txt");
    if (message_file.is_open()) {
      getline(message_file, message);
      message_file.close();
    } else {
      cout << "Error: Unable to open secret_message.txt file." << endl;
      return -1;
    }

    // Prompt user to enter a password for image encryption
    cout << "Enter a password to encrypt the image: ";
    string image_password;
    cin >> image_password;

    // Compute hash of image password
    unsigned char hashed_image_password[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char * ) image_password.c_str(), image_password.length(), hashed_image_password);
    string hashed_image_password_str((const char * ) hashed_image_password, SHA256_DIGEST_LENGTH);

    // Store hashed image password
    store_hashed_password(hashed_image_password_str);

    // Encrypt message with image password
    string encrypted_message;
    int key_length = hashed_image_password_str.length();
    int message_length = message.length();

    for (int i = 0; i < message_length; i++) {
      int key_index = i % key_length;
      int shift = hashed_image_password_str[key_index] - 'a';
      char c = message[i] + shift;
      if (c > 'z') {
        c -= 26;
      }
      encrypted_message += c;
    }

    // Write encrypted message to file
    ofstream encrypted_file;
    encrypted_file.open("encrypted_message.txt");
    encrypted_file << encrypted_message;
    encrypted_file.close();

    // Encrypt image with hashed image password
    Mat encrypted_image;
    bitwise_xor(image, Scalar(std::bitset < 8 > (hashed_image_password_str[0]).to_ulong()), encrypted_image);
    for (int i = 1; i < hashed_image_password_str.length(); i++) {
      bitwise_xor(encrypted_image, Scalar(std::bitset < 8 > (hashed_image_password_str[i]).to_ulong()), encrypted_image);
    }

    // Save encrypted image
    imwrite("encrypted.png", encrypted_image);

    // Prompt user to enter password to decrypt message
    cout << "Enter the password to decrypt the message: ";
    string decrypt_password;
    cin >> decrypt_password;

    // Compute hash of decryption password
    unsigned char hashed_decrypt_password[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char * ) decrypt_password.c_str(), decrypt_password.length(), hashed_decrypt_password);
    string hashed_decrypt_password_str((const char * ) hashed_decrypt_password, SHA256_DIGEST_LENGTH);

    // Decrypt message with decryption password
    decrypt(encrypted_message, hashed_decrypt_password_str, hashed_image_password_str, hashed_password_input_str);

    return 0;
