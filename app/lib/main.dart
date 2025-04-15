import 'package:flutter/material.dart';

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
class Kontrollpanel extends StatelessWidget {
  const Kontrollpanel({super.key});

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
                  ElevatedButton.icon(
                    onPressed: () {
                      // Logikk for tilkobling
                      print("Kobler til kontroller...");
                    },
                    icon: const Icon(Icons.bluetooth),
                    label: const Text(
                      'Koble til kontroller',
                      style: TextStyle(
                        color: Colors.white,
                        fontSize: 14,
                      ),
                      ),
                    style: ElevatedButton.styleFrom(
                      backgroundColor: Colors.deepPurpleAccent,
                      padding: const EdgeInsets.symmetric(vertical: 14, horizontal: 20),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(30),
                      ),
                    ),
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
          ],
        ),
      ),
    );
  }
}