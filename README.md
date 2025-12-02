Jest to repozytorium i branch "main" przeznacozny do pracy w programie platformio. 

 Projekt: Porgoramator do Grennpacków
  Opis: Jest to projekt do obsługi GreenPaków posiada takie funkcje jak odczyt, zapis i wyczyszczenie pamięci GreenPacka.
        Poruszać się można w dwóch trybach: automatycznym- jedna komenda zawierająca wiele rozkazów lub przycisk do programowania pamięci NVM GreenPacka z pamięci Arduino EEPROM, manulanym- każda komenda jeden rozkaz.
        OPIS ROZKAZÓW:
        a-uruchom tryb automatyczny (domyślnie po uruchomieniu w trybie automatycznym).
        m- uruchom tryb manulany.
        r- read, odczytywanie pamięci.
        e- erase, czyszczenie pamięci.
        w - write, zapis danych do pamięci.
        p- ping, sprawdzenie aktywnych adresów greenpaków (tylko w trybie manualnym).
        <0,f>U{x} - wpisywanie adresu greenpacka z którym chcemy się skomunikować. x- pierwszy adres automatycznie odczytany.
        <0,f>U{x,p} - wpisywanie nowego adresu greenpacka po zapisie pamięci NVM, x-pozostawienie automatycznie odczytanego adresu, p-pozostawienie adresu ustawionego z wpisywanego programu.
        n- wybor pamieci NVM greenpacka- non volatile memory (pamięć do wgrania programu greenpacka).
        e- wybor pamieci EEPROM greeenpacka.
        m- memory, wybor zrodla danych do zapisu pamięci GreenPacka, pamięć Arduino.
        s- serial, wybor zrodla danych do zapisu pamięci GreenPacka, zapis danych z serial monitora (komunikacja UART).
        f- Arduino FLASH, dalszy wybór źródła danych z pamięci Arduino.
        a- Arduino EEPROM, dalszy wybór źródła danych z pamięci Arduino ('a' jeszcze jedno znaczenie w trybie read).
        i- ignore, zignoruj zapis nowo wprowadzonego programu z serial monitora do EEPROMu Arduino.
        u- update, uaktualnij zapis nowo wprowadzonego programu z serial monitora do EEPROMu Arduino.
        g- Greenpack, wybór Greenpacka, urządzenia, z którego chcesz odczytać pamięć.
        a- Arduino, wybór Arduino, urządzenia, z którego chcesz odczytać pamięć.
        Przykłady komend w trybie automatucznym:
        wxnmfxi - write, x-pierwszy odczytany adres greenpacka, n-zapis do NVM, m-dane z pamieci arduino, f-dane z pamiec flash, x-pozostaw pierwszy odczytany adres greenpacka po programowaniu, i- zignoruj updateeeprom Arduino
        exn- erase, x-pierwszy odczytany adres greenpacka, n- wyczyscp pamiec NVM
        r1nga- read, 1-odczyt greenpacka o Adresie 0x01, n- odczyt pamięci, g-odczyt z Greenpacka
        WAŻNE: w trybie autmatycznym komenda zapisu musi skladac sie z 7 znaków, czyszczenia z 3, odczytu z 5.
               Jeśli komenda nie będzie korzystała z wszystkich rozkazów i tak trzeba wpisać tyle znaków do poprawnego formatu komendy.
               ZAPIS:[w][addr/x][n/e][m/s][f/a][new addr/x/p][i/u]
               ODCZYT:[r][addr/x][n/e][a/g][f/a]
               CZYSZCZENIE: [e][addr/x][n/e]
