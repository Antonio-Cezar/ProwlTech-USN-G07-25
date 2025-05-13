import 'package:flutter/material.dart';     // Flutter sitt UI-bibliotek
import 'package:http/http.dart' as http;    // HTTP-klient for å hente data fra Flask-API
import 'dart:convert';                      // JSON-dekoder for å tolke API-svar
import 'dart:async';                        // For periodisk oppdatering

// Hovedmetode som starter Flutter-appen
void main() {
  runApp(const ProwlTechApp());
}

// Hovedoppsettet til appen
class ProwlTechApp extends StatelessWidget {
  const ProwlTechApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ProwlTech Kontrollpanel',   // App-tittel
      home: const Kontrollpanel(),        // Første skjerm som vises
      debugShowCheckedModeBanner: false,  // Fjerner debug-banneret øverst
    );
  }
}

// Innholdet i appen
class Kontrollpanel extends StatefulWidget {    // StatefulWidget: skal hente og oppdatere data løpende
  const Kontrollpanel({super.key});

  @override
  State<Kontrollpanel> createState() => _KontrollpanelState();
}

// Tilstanden til kontrollpanel: holder data og bygger UI
class _KontrollpanelState extends State<Kontrollpanel> {
  bool connected = false;   // Viser om kontrolleren er tilkoblet

// Metode som henter status fra Flask-serveren
  Future<void> getStatus() async {
    print('Prøver å hente status...');  // Debug

    try {
      // Sender GET-forespørsel
      final response = await http.get(
        Uri.parse('http://192.168.137.6:5000/status'),
      );

      // Sjekker om OK
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);   // Dekoder JSON til et Dart-objekt
        print('Data fra API: $data');

        // Oppdaterer state med ny data
        setState(() {
          connected = data['kontroller_tilkoblet'];
        });
      } 
      else {
        print('Feil ved henting: ${response.statusCode}');  // Logger feilkode hvis ikke 200
      }
    } 
    catch (e) {
      print('Klarte ikke å hente status: $e');  // Logger unntak (timeout eller nettverksfeil)
    }
  }

  @override
  void initState() {
    super.initState();
    getStatus();     // Henter status ved oppstart
    // Henter status hvert 5. sekund
    Timer.periodic(const Duration(seconds: 5), (timer) {
      getStatus();
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1A0033),            // Bakgrunnsfarge for hele skjermen
      body: Padding(
        padding: const EdgeInsets.all(16.0),                 // 16px padding rundt alt
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,   // Juster innhold til venstre
          children: [
            const SizedBox(height: 50),                     // Tom plass øverst

            //------------------------------TITTELBOKS---------------------------------
            Container(
              padding: const EdgeInsets.fromLTRB(40, 16, 16, 16),    // Luft inni boksen
              decoration: BoxDecoration(
                color: const Color(0xFF2E1458),                // Lilla bakgrunn
                borderRadius: BorderRadius.circular(8),         // Runde hjørner
              ),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                crossAxisAlignment: CrossAxisAlignment.center,
                children: [
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: const[
                      Text(
                        'ProwlTech',                         // Hovedtittel
                        style: TextStyle(
                          fontSize: 40,                     // Tekststørrelse
                          color: Colors.white,            // Tekstfarge
                          fontWeight: FontWeight.bold,      // Fet skrift
                        ),
                      ),
                      SizedBox(height: 1),            // Litt luft 
                      Text(
                        'Kontrollpanel',                    // Undertittel
                        style: TextStyle(
                          fontSize: 36,                     // Tekststørrelse
                          color: Colors.purpleAccent,    // Tekstfarge
                          fontWeight: FontWeight.w500,
                        ),
                      ),
                    ],
                  ),

//------------------------------INFO-KNAPP---------------------------------
                  OutlinedButton.icon(
                    onPressed: () {
                      // Viser et popup-vindu ved knappetrykk
                      showDialog(
                        context: context,   // Viser popup i riktig del av appen
                        builder: (context) => AlertDialog(
                          backgroundColor: const Color(0xFF2E1458),   // Bakgrunnsfarge
                          title: const Text(
                            'Om bilen',                                 // Tittel i popup
                            style: TextStyle(color: Colors.white),    // Tekstfarge
                          ),
                          content: const Text(
                            'Bilen styres via en trådløs kontroller. Sørg for at CAN-bus er aktiv før srart.',                       // Infotekst i popup
                            style: TextStyle(color: Colors.white70),    // Tekstfarge
                          ),
                          actions: [
                            // Knapp nederst i popup for å lukke vinduet
                            TextButton(
                              onPressed: () => Navigator.pop(context),    // Lukk popup
                              child: const Text('OK'),                    // Tekst på knapp
                            ),
                          ],
                        ),
                      );
                    },
                    // Stil på knappen
                    style: OutlinedButton.styleFrom(
                      side: const BorderSide(color: Colors.white70),    // Rammefarge på knappen
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(12),    // Runde hjørner
                      ),
                      // Luft inni knappen
                      padding: const EdgeInsets.symmetric(
                        horizontal: 12,
                        vertical: 8,
                      ),
                    ),
                    icon: const Icon(Icons.info_outline, color: Colors.white),    // Ikon på venstre side av teksten
                    label: const Text(
                      'Info',                                     // Tekst på knappen
                      style: TextStyle(color: Colors.white),    // Tekstfarge
                    ),
                  ),
                ],
              ),
            ),

            const SizedBox(height: 24),   // Luft mellom elementer

            //------------------------------KONTROLLERBOKS---------------------------------
            Container(
              padding: const EdgeInsets.all(16),                 // Luft inni boksen på alle kanter
              decoration: BoxDecoration(
                color: const Color(0xFF4A3C6E),               // Mørk lilla bakgrunn
                borderRadius: BorderRadius.circular(12),        // Runde hjørner
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,   // Juster innhold til venstre
                children: [
                  const Text(
                    'KONTROLLER',   // Tittel
                    style: TextStyle(
                      color: Colors.white,                  // Tekstfarge
                      fontSize: 18,                           // Tekststørrelse
                      fontWeight: FontWeight.bold,            // Fet skrift
                    ),
                  ),
                  const SizedBox(height: 8),                  // Luft mellom elementer

                  // Rad med ikon og statustekst
                  Row(
                    children: [
                      Icon(
                        // Viser grønt ikon hvis tilkoblet, ellers rødt kryss
                        kontrollerTilkoblet
                            ? Icons.check_circle
                            : Icons.cancel,
                        color: kontrollerTilkoblet
                            ? Colors.green
                            : Colors.red,
                      ),
                      const SizedBox(width: 8),                        // Luft mellom elementer
                      Text(
                        // Viser tekst avhengig av tilkoblingsstatus
                        kontrollerTilkoblet
                            ? 'Kontroller tilkoblet'
                            : 'Kontroller ikke tilkoblet',
                        style: const TextStyle(
                          color: Colors.white,                      // Tekstfarge
                          fontSize: 16,                               // Tekststørrelse
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),

            const SizedBox(height: 24),                               // Luft mellom elementer

            //------------------------------BATTERI OG SENSOR---------------------------------
            Row(
              children: [
                Expanded(
                  child: Container(
                    padding: const EdgeInsets.all(16),                 // Luft inni boksen på alle kanter
                    decoration: BoxDecoration(
                      color: const Color(0xFF7E5ABD),
                      borderRadius: BorderRadius.circular(12),        // Runde hjørner
                    ),
                    child: const Column(
                      crossAxisAlignment: CrossAxisAlignment.start,   // Juster innhold til venstre
                      children: [
                        Text(
                          'BATTERI',   // Tittel
                          style: TextStyle(
                            color: Colors.white,                  // Tekstfarge
                            fontSize: 18,                           // Tekststørrelse
                            fontWeight: FontWeight.bold,            // Fet skrift
                          ),
                        ),
                        SizedBox(height: 8),                        // Luft mellom elementer
                        Text(
                          '---%',
                          style: TextStyle(
                            color: Colors.white70,                // Tekstfarge
                          ),
                        ),
                        SizedBox(height: 60),                       // Luft mellom elementer
                      ],
                    ),
                  ),
                ),
                const SizedBox(width: 16),                         // Luft mellom elementer
                Expanded(
                  child: Container(
                    padding: const EdgeInsets.all(16),             // Luft inni boksen på alle kanter
                    decoration: BoxDecoration(
                      color: const Color(0xFF7E5ABD),
                      borderRadius: BorderRadius.circular(12),     // Runde hjørner
                    ),
                    child: const Column(
                      crossAxisAlignment: CrossAxisAlignment.start,   // Juster innhold til venstre
                      children: [
                        Text(
                          'SENSORDATA',                             // Tittel
                          style: TextStyle(
                            color: Colors.white,                 // Tekstfarge
                            fontSize: 18,                          // Tekststørrelse
                            fontWeight: FontWeight.bold,           // Fet skrift
                          ),
                        ),
                        SizedBox(height: 8),                      // Luft mellom elementer
                        Text(
                          'Foran:\nBak:\nHøyre:\nVenstre:',
                          style: TextStyle(
                            color: Colors.white70,              // Tekstfarge
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
              ],
            ),

            const SizedBox(height: 24),   // Luft mellom elementer

            //------------------------------SYSTEMLOGG---------------------------------
            Container(
              padding: const EdgeInsets.all(16),    // Luft inni boksen på alle kanter
              decoration: BoxDecoration(
                color: const Color(0xFF8B2C39),   // Rød bakgrunnsfarge
                borderRadius: BorderRadius.circular(12),    // Runde hjørner
              ),

              // Innholdet i boksen legges i en kolonne (under hverandre)
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,   // Juster innhold til venstre
                children: [
                  const Text(
                    'SYSTEMLOGG',   // Tittel
                    style: TextStyle(
                      color: Colors.white,    // Tekstfarge
                      fontWeight: FontWeight.bold,    // Fet skrift
                      fontSize: 16,   // Tekststørrelse
                    ),
                  ),
                  const SizedBox(height: 12),   // Luft under tittelen
                  Text(
                    feilmeldinger.isEmpty   // Hvis lista er tom
                        ? 'Ingen meldinger'
                        : feilmeldinger.map((e) => '• $e').join('\n'),    // Ellers lag punktliste
                    style: const TextStyle(
                      color: Colors.white70,    // Tekstfarge
                      fontSize: 14,   // Tekststørrelse
                    ),
                  ),
                ],
              ),
            ),

            const SizedBox(height: 24),   // Luft mellom elementer

            //------------------------------BILDE AV BILEN---------------------------------
            Container(
              decoration: BoxDecoration(
                color: const Color(0xFF4A3C6E),   // Mørk lilla bakgrunn
                borderRadius: BorderRadius.circular(12),    // Runde hjørner
              ),
              padding: const EdgeInsets.all(16),    // Luft inni boksen på alle kanter
              child: Image.asset(
                'images/prowltech_car.png',   // Stien til bildet
                fit: BoxFit.contain,    // Bildet skal ikke bli kuttet
                height: 160,    // Høyde på bildet
              ),
            ),
          ],
        ),
      ),
    );
  }
}
