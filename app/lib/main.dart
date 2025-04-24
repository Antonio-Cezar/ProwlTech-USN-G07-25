import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'dart:async';


void main() {
  runApp(const ProwlTechApp());
}

// Hovedappen
class ProwlTechApp extends StatelessWidget {
  const ProwlTechApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ProwlTech Kontrollpanel',
      home: const Kontrollpanel(),
      debugShowCheckedModeBanner: false,
    );
  }
}

// Innholdet i appen
class Kontrollpanel extends StatefulWidget {
  const Kontrollpanel({super.key});

  @override
  State<Kontrollpanel> createState() => _KontrollpanelState();

}

class _KontrollpanelState extends State<Kontrollpanel> {
  bool kontrollerTilkoblet = false; // Mock-tilstand
  List<String> feilmeldinger = []; 

  Future<void> hentStatus() async {
    try {
      final response = await http.get(
        Uri.parse('http://192.168.137.113:5000/status'),
      );

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
          crossAxisAlignment: CrossAxisAlignment.stretch,   // Gjør at innholdet fyller hele plassen
          children: [
            const SizedBox(height: 50),   // Tom plass øverst

            // Tittelboks
            Container(
              padding: const EdgeInsets.symmetric(vertical: 24),    // Luft over og under inni boksen
              decoration: BoxDecoration(
                color: const Color(0xFF2E1458),   // Bakgrunnsfarge i boksen
                borderRadius: BorderRadius.circular(8),   // Avrunder hjørnene
              ),
              // Innholdet i boksen
              child: const Column(
                children: [
                // Første linje i tekst
                  Text(
                    'ProwlTech',
                    style: TextStyle(
                      fontSize: 32,
                      color: Colors.white,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  SizedBox(height: 8),    // Avstand mellom tekstlinjene
                  // Andre linje i tekst
                  Text(
                    'Kontrollpanel',
                    style: TextStyle(
                      fontSize: 26,
                      color: Colors.purpleAccent,
                      fontWeight: FontWeight.w500,
                    ),
                  )
                ],
              ),
            ),
            const SizedBox(height: 24),   // Mellomrom mellom tittelboks og neste boks
            // ------------------------------------------------------------------------------------------Kontrollerboks
            Container(
              padding: const EdgeInsets.all(16),   // Luft inni boksen
              decoration: BoxDecoration(
                color: Color(0xFF4A3C6E),
                borderRadius: BorderRadius.circular(12),    // Avrundet hjørner
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,   // Tekst starter til venstre
                children: [
                  Text(
                    'Kontroller:',
                    style: TextStyle(
                      color: Colors.white,
                      fontSize: 18,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                  SizedBox(height: 8),  // Luft mellom linjene
                  Text(
                    'Input',
                    style: TextStyle(
                      color: Colors.white70,  // Lys grå tekst
                      fontSize: 16,
                    ),
                  ),
                  const SizedBox(height: 16),
                  // Knapp (Koble til kontroller)
                 Row(
                  children: [
                    Icon(
                      kontrollerTilkoblet ? Icons.check_circle : Icons.cancel,
                      color: kontrollerTilkoblet ? Colors.green : Colors.red,
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
            // Nye rad for batteri- og sensorboks
            Row(
              children: [
                // ------------------------------------------------------------------------------------------Batteriboks
                Expanded(
                  child: Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: Color(0xFF5E4B8B),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: const Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          'BATTERI',
                          style: TextStyle(
                            color: Colors.white,
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
                // ------------------------------------------------------------------------------------------Sensorboks
                Expanded(
                  child: Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: Color(0xFF7E5ABD),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: const Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          'SENSORDATA',
                          style: TextStyle(
                            color: Colors.white,
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
                // ------------------------------------------------------------------------------------------Feilmeldingsboks
                Container(
                  padding: const EdgeInsets.all(16),
                  decoration: BoxDecoration(
                    color: Color(0xFF8B2C39),
                    borderRadius: BorderRadius.circular(12),
                  ),
                  child: const Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(
                        'FEILMELDINGER',
                        style: TextStyle(
                          color: Colors.white,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                      SizedBox(height: 12),
                      Text(
                        'Ingen feilmeldinger',
                        style: TextStyle(
                          color: Colors.white70,
                          fontSize: 14,
                          ),
                      ),
                    ],
                  ),
                 ),
          ],
        ),
      ),
    );
  }
}