### Download ns3
```bash
wget https://www.nsnam.org/releases/ns-allinone-3.44.tar.bz2
```

### Extract the ns3
```bash
tar xvf ns-allinone-3.44.tar.bz2
```

### Install dependencies if any error:
```bash
sudo apt update
sudo apt install cmake cmake-doc build-essentials qmake6 qconf
```
### Post extracting, move into the ns3 folder/directory

```bash
cd ns-allinone-3.44
```

### Build the ns3

```bash
./build.py --enable-examples --enable-tests
```

### basic commands to copy and change/move folders/file/directory
```bash
mv ns3 ns3-program # this should rename ns3 folder to ns3-program
cp examples/wireless/wifi-simple-adhoc scratch/ # this should copy the file to scratch folder notice that you're in ns3 folder already
```
