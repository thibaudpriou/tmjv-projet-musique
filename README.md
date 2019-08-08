# Projet d'analyse musique TMJV


```sudo apt install libflac-dev libvorbis-dev gnuplot```

Pour installe FFTW :
```
wget http://fftw.org/fftw-3.3.8.tar.gz
tar -xzf fftw-3.3.8.tar.gz
cd fftw-3.3.8/
./configure
make
sudo make install
```


## But
Sortir des informations d'un fichier WAV. Ces informations sont celles qui ont été vus en cours et mais aussi toutes celles ce qui peuvent être jugées utiles.

Interface : GUI ou CLI au choix.

Exemples d'informations :

- Tempo (codé)
- Tonalité (codé)
- Suite d'accords 
- Spectrogramme
- Interprétation musicale
- auto-similarité (codé)

