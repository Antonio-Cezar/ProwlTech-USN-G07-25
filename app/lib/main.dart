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
      home: const Kontrollpanel(),        // Første skjerm-widget
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

// Tilstanden til kontrollpanel: lagrer API-data og bygger
class _KontrollpanelState extends State<Kontrollpanel> {
  bool kontrollerTilkoblet = false;   // Viser om kontrolleren er tilkoblet
  List<String> feilmeldinger = [];    // Feilmeldinger fra API

// Metode som henter status fra Flask-serveren
  Future<void> hentStatus() async {
    try {
      // Sender GET-forespørsel
      final response = await http.get(
        Uri.parse('http://192.168.137.113:5000/status'),
      );

      // Sjekker om OK
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        setState(() {
          kontrollerTilkoblet = data['kontroller_tilkoblet'];
          feilmeldinger = List<String>.from(data['feilmeldinger']);
        });
      } else {
        print('Feil ved henting: ${response.statusCode}');
      }
    } catch (e) {
      print('Klarte ikke å hente status: $e');
    }
  }

  @override
  void initState() {
    super.initState();
    hentStatus();
    Timer.periodic(const Duration(seconds: 5), (timer) {
      hentStatus();
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1A0033),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            const SizedBox(height: 50),

            // Tittelboks
            Container(
              padding: const EdgeInsets.symmetric(vertical: 24),
              decoration: BoxDecoration(
                color: const Color(0xFF2E1458),
                borderRadius: BorderRadius.circular(8),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Text(
                    'ProwlTech',
                    style: TextStyle(
                      fontSize: 40,
                      color: Colors.white,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  const SizedBox(height: 1),
                  const Text(
                    'Kontrollpanel',
                    style: TextStyle(
                      fontSize: 36,
                      color: Colors.purpleAccent,
                      fontWeight: FontWeight.w500,
                    ),
                  ),
                  const SizedBox(height: 20),

                  OutlinedButton.icon(
                    onPressed: () {
                      showDialog(
                        context: context,
                        builder: (context) => AlertDialog(
                          backgroundColor: const Color(0xFF2E1458),
                          title: const Text(
                            'Om bilen',
                            style: TextStyle(color: Colors.white),
                          ),
                          content: const Text(
                            'Kort info om bilen',
                            style: TextStyle(color: Colors.white70),
                          ),
                          actions: [
                            TextButton(
                              onPressed: () => Navigator.pop(context),
                              child: const Text('OK'),
                            ),
                          ],
                        ),
                      );
                    },
                    style: OutlinedButton.styleFrom(
                      side: const BorderSide(color: Colors.white70),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(12),
                      ),
                      padding: const EdgeInsets.symmetric(
                        horizontal: 12,
                        vertical: 8,
                      ),
                    ),
                    icon: const Icon(Icons.info_outline, color: Colors.white),
                    label: const Text(
                      'Info',
                      style: TextStyle(color: Colors.white),
                    ),
                  ),
                ],
              ),
            ),

            const SizedBox(height: 24),

            // Kontrollerboks
            Container(
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: const Color(0xFF4A3C6E),
                borderRadius: BorderRadius.circular(12),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Text(
                    'KONTROLLER',
                    style: TextStyle(
                      color: Colors.white,
                      fontSize: 18,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  const SizedBox(height: 8),

                  // Tilkoblingsstatus
                  Row(
                    children: [
                      Icon(
                        kontrollerTilkoblet
                            ? Icons.check_circle
                            : Icons.cancel,
                        color: kontrollerTilkoblet
                            ? Colors.green
                            : Colors.red,
                      ),
                      const SizedBox(width: 8),
                      Text(
                        kontrollerTilkoblet
                            ? 'Kontroller tilkoblet'
                            : 'Kontroller ikke tilkoblet',
                        style: const TextStyle(
                          color: Colors.white,
                          fontSize: 16,
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),

            const SizedBox(height: 24),

            // Batteri- og sensorboks
            Row(
              children: [
                Expanded(
                  child: Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: const Color(0xFF7E5ABD),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: const Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          'BATTERI',
                          style: TextStyle(
                            color: Colors.white,
                            fontSize: 18,
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                        SizedBox(height: 8),
                        Text(
                          '---%',
                          style: TextStyle(
                            color: Colors.white70,
                          ),
                        ),
                        SizedBox(height: 60),
                      ],
                    ),
                  ),
                ),
                const SizedBox(width: 16),
                Expanded(
                  child: Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: const Color(0xFF7E5ABD),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: const Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          'SENSORDATA',
                          style: TextStyle(
                            color: Colors.white,
                            fontSize: 18,
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                        SizedBox(height: 8),
                        Text(
                          'Foran:\nBak:\nHøyre:\nVenstre:',
                          style: TextStyle(
                            color: Colors.white70,
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
              ],
            ),

            const SizedBox(height: 24),

            // Feilmeldingsboks
            Container(
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: const Color(0xFF8B2C39),
                borderRadius: BorderRadius.circular(12),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Text(
                    'FEILMELDINGER',
                    style: TextStyle(
                      color: Colors.white,
                      fontWeight: FontWeight.bold,
                      fontSize: 16,
                    ),
                  ),
                  const SizedBox(height: 12),
                  Text(
                    feilmeldinger.isEmpty
                        ? 'Ingen feilmeldinger'
                        : feilmeldinger.map((e) => '• $e').join('\n'),
                    style: const TextStyle(
                      color: Colors.white70,
                      fontSize: 14,
                    ),
                  ),
                ],
              ),
            ),

            const SizedBox(height: 24),

            // Bilde av bilen
            Container(
              decoration: BoxDecoration(
                color: const Color(0xFF4A3C6E),
                borderRadius: BorderRadius.circular(12),
              ),
              padding: const EdgeInsets.all(16),
              child: Image.asset(
                'images/prowltech_car.png',
                fit: BoxFit.contain,
                height: 160,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
