# libsolana

To get the Solana C++ SDK, you can follow the instructions in the Solana documentation. Here is a summary of the steps you need to follow:

1. Install the dependencies for the Solana C++ SDK by running the following command:

```
sudo apt update
sudo apt install -y build-essential cmake pkg-config libssl-dev
```

2. Clone the Solana C++ SDK repository from GitHub:

```
git clone https://github.com/many-exchange/solana-sdk-cpp.git
```

3. Navigate to the repository directory:

```
cd solana-sdk-cpp
```

4. Build and install the Solana C++ SDK:

```
make
sudo make install
```

This will build the Solana C++ SDK and install it on your system. You can then use the SDK in your C++ programs by including the solana.h header file.
