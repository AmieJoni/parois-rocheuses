# Introduction
Ce Rapport technique présente les outils utilisés ainsi que la démarche suivie pour la réalisation de ce projet,
ce dernier a pour but d'envoyer la mesure faite par notre dispositif de la distance des parois rocheuses par le biais du réseau LoraWan.

# Outils
Dans cette partie on va présenter les outils qu'on a utilisés.

## STM32 IoTNode B-L475E-IOT01A
La carte électronique STM32L4 est un kit de développement des systèmes embarqués qui permet la réalisation de divers types d'application en utilisant des capteurs et dispositifs intégrés : NFC, Wifi, humidity & temperature, magnétomètre...
Cette carte est compatible arduino, du coup pour le développement on utilisera l'environnement d'arduino. De plus la carte est extensible, on y ajoutera un module LoRaWAN pour la communication réseau.

## VL53L0X
La carte STM32L4 embarque aussi le capteur VL53L0X qui utilise une variation de la technique du LIDAR ToF, sa vision est jusqu'à 1 m et il est plus précis, ce dernier on l'utilisera pour mesurer la distance des parois rocheuses.

## NodeRED
NodeRED est un canevas de traitement de flots de données. Il permet la programmation visuelle des chaines de traitement de données. Il est très populaire pour le prototypage d'applications IoT.

## InfluxDB
InfluxDB est un système de gestion de base de données orientée séries chronologiques hautes performances.

## Grafana
Grafana est un logiciel libre qui permet la visualisation de données.

# Démarche
Dans cette partie on va vous montrer la démarche qu'on a suivie pour réaliser le projet.

## Aquisition de la mesurer
Pour l'acquisition de la mesure on on utilise la librairie vl53l0x, elle nous permet d'interagir avec le capteur et connaître son état de prise de la mesure.
  uint32_t distance;
    int status;
    status = sensor_vl53l0x.GetDistance(&distance);

    while(status != VL53L0X_ERROR_NONE){
      status = sensor_vl53l0x.GetDistance(&distance);
    }
    Serial.println(distance);
On obtiendra la mesure dans la variable "distance" qui a une taille de 32 bits, la mesure en question représente la distance en millimètres.

## Transformation de la mesure
Pour envoyer la mesure dans le réseau la contrainte est d'envoyer byte/byte. Notre mesure est prise dans une taille de 4 bytes (32 bits) il faut la divisée.
		for(int i=0; i<4; i++) {
			if(i != 0){
				distance = distance >> 8;
			}
			byte data = distance & 255;
			frameTx[i] = data;
		}
Dans une boucle en utilisant les opérateurs binaires on fait une rotation à droite de 8 bits pour ramener les bits les plus significatifs puis un 'ET' logique avec la valeur 255 pour en garder la valeur des 8 premiers bits seulement.
Finalement on stocke les bytes dans un tableau de caractère.

## Envoi de la mesure
Pour l'envoi de la mesure dans le réseau on a créé une application et à l'intérieur de celle-ci on a ajouté le notre périphérique d'envoi.

### Enregistrement
Un équipement réseau doit être enregistré dans un seul réseau LoRaWAN seulement, un équipement est identifié par son DevUEI
c'est comme l'adresse MAC d'un ordinateur il est unique et propre au matériel.

### Envoi
On utilisera la librairie 'LoRaWANNode' pour contrôler la carte réseau LoRaWAN à notre disposition.
L'envoi des données dans le réseau passe par 3 étapes:
- Etape 1: Préparation du module
		Serial.println("-- LoRaWAN OTAA sketch --");
		// Enable the USI module and set the radio band.
		while(!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
		Serial.println("Lora module not ready");
			delay(1000);
		}

- Etape 2: Envoi d'une demande d'admission au serveur
		// Send a join request and wait the join accept
		while(!loraNode.joinOTAA(appKey, appEUI)) {
			Serial.println("joinOTAA failed!!");
			delay(1000);
		}

- Etape 2: Envoi des données sur le réseau
		int status = loraNode.sendFrame(frameTx, sizeof(frameTx), UNCONFIRMED);
		if(status == LORA_SEND_ERROR) {
		Serial.println("Send frame failed!!!");
			} else if(status == LORA_SEND_DELAYED) {
			Serial.println("Module busy or duty cycle");
		} else {
			Serial.println("Frame sent");
		}

### Decodage
Dans le serveur LoRaWAN on doit mettre en place une fonction javascript pour décoder les bytes reçus, parce que celui qui reçois les bytes ne sait pas ce qu'elles présentent.

- C'est la fonction de décodage qui reçoit les bytes.
		function Decode(fPort, bytes) {
			if(bytes.length < 4) {
				return {"distance": -1};
			}
				return {"distance": readUInt32LE(bytes,0)}
		}

- Cette fonction lit un entier dans 32bits (4 bytes)
		function readUInt32LE (buf, offset) {
			offset = offset >>> 0;
			return ((buf[offset]) | (buf[offset + 1] << 8) |(buf[offset + 2] << 16)) +(buf[offset + 3] * 0x1000000);
		}

## Visualisation
Dans cette partie c'est plus comment récupérer les mesures des capteurs gérés par la plateforme CampusIoT afin de les archiver et de les visualiser en temps réel au moyen de la stack NodeRED, InfluxDB, Grafana.
Puisque c'est plutôt de la configuration, cette partie sera expliquée dans la vidéo qui accompagnera se rapport.
