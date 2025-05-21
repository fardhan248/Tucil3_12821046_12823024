# Rush Hour Solver

## Deskripsi
**Rush Hour** adalah sebuah permainan puzzle logika berbasis grid yang menantang pemain untuk 
menggeser kendaraan di dalam sebuah kotak (biasanya berukuran 6x6) agar mobil utama 
(biasanya berwarna merah) dapat keluar dari kemacetan melalui pintu keluar di sisi papan. 
Setiap kendaraan hanya bisa bergerak lurus ke depan atau ke belakang sesuai dengan 
orientasinya (horizontal atau vertikal), dan tidak dapat berputar. Tujuan utama dari permainan 
ini adalah memindahkan mobil merah ke pintu keluar dengan jumlah langkah seminimal 
mungkin. 


Tujuan utama dari pembuatan program ini adalah untuk menemukan solusi dari permainan Rush Hour yang diberikan sesuai algoritma yang dipilih.

## Kompilasi program (Linux/WSL)
1. Buka Ubuntu dan pindah ke direktori project berada:
```bash
cd path\to\RushHourSolver
```

2. Jalankan perintah berikut untuk kompilasi program:
```bash
g++ main.cpp -o main
```

3. Akan muncul file dengan nama `main`.

## Cara menjalankan program
Di direktori yang memiliki file `main`, jalankan perintah berikut
```bash
./main <nama_file.txt> <algoritma>
```

Keterangan:
- nama file.txt: berisi konfigurasi papan permainan Rush Hour dengan format
```txt 
A B
N
konfigurasi_papan 
```

dengan:
- A dan B: ukuran papan (baris, lebar)
- N: banyak kendaraan (selain kendaraan utama)
- konfigurasi papan dengan contoh lengkap:
```txt 
6 6
11
AAB..F
..BCDF
GPPCDFK
GH.III
GHJ...
LLJMM.
```

Berikut merupakan algoritma yang dapat dipilih <br>
- ucs: Uniform Cost Search (UCS)
- gbfs: Greedy Best First Searh (GBFS)
- astar: A*

# Author
- 12821046: Fardhan Indrayesa
- 12823024: Azadi Azhrah