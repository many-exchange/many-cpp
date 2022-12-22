# Solana C++ SDK

To get the Solana C++ SDK, you can follow the instructions in the Solana documentation. Here is a summary of the steps you need to follow:

1. Install the dependencies for the Solana C++ SDK by running the following command:

```
sudo apt update
sudo apt install libssl-dev
sudo apt install nlohmann-json3-dev
```

2. Clone the Solana C++ SDK repository from GitHub:

```
git clone https://github.com/many-exchange/solana-sdk-cpp.git
```

3. Navigate to the repository directory:

```
cd solana-sdk-cpp
```

4. Test the Solana C++ SDK:

```
make test
```

This will run the Solana C++ SDK unit tests.

5. Install the Solana C++ SDK:

```
sudo make install
```

This will install the Solana C++ SDK on your system. You can then use the SDK in your C++ programs by including the solana.h header file.
