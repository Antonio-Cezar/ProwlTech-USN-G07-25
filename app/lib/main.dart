import 'package:flutter/material.dart';

void main() {
  runApp(const ProwlTechApp());
}

// Hovedappen (det som holder alt innholdet)
class ProwlTechApp extends StatelessWidget {
  const ProwlTechApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ProwlTech Kontrollpanel',   // Navnet på appen
      home: const Kontrollpanel(),        // Første skjerm som vises ved oppstart
      debugShowCheckedModeBanner: false,  // Fjerner debug-banner
    );
  }
}

// Innholdet i appen
class Kontrollpanel extends StatelessWidget {
  const Kontrollpanel({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1A0033),   // Bakgrunnsfarge
      body: Padding(
        padding: const EdgeInsets.all(16.0),        // Litt luft rundt hele innholdet
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,   // Gjør innholdet så bredt som mulig
          children: const [
            SizedBox(height: 50),   // Tom plass øverst
            Center(
              child: Text(
                'ProwlTech',    // Hovedtittel
                style: TextStyle(
                  fontSize: 46,
                  color: Colors.white,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),
            Center(
              child: Text(
                'Kontrollpanel',    // Undertittel
                style: TextStyle(
                  fontSize: 36,
                  color: Colors.purpleAccent,
                  fontWeight: FontWeight.w500,
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
