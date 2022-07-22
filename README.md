# Projeto desenvolvimento de aplicação utilizando Bluetooth

Este repositório contém os arquivos e especificações do projeto final da disciplina de Internet das Coisas (IOT). O projeto escolhido foi: "Desenvolvimento de aplicação utilizando Bluetooth". As seguintes funcionalidades devem ser implementadas:

* Conexão WiFi:
    * Atualização com biblioteca OTA (via WiFi).
* Conexão bluetooth:
    * A conexão WiFi deve ser ativada pelo bluetooth (economia de energia);
    * O usuário deve ser capaz de comandar o LED a ligar/desligar (via bluetooth);
    * Dados de umidade devem ser enviados com periodicidade (via bluetooth).
* Ler dados de umidade do sensor.

O módulo a ser utilizado será o ESP32, pois possui capacidades de conexão via WiFi e bluetooth sem que seja necessária a adição de outros componentes.

## Planejamento

Segue o planejamento quanto à adiação de funcionalidades:

[X] Documentar a [especificação da aplicação](#doc_app);<br/>
[ ] Documentar o [ambiente de desenvolvimento](#doc_dev);<br/>
[ ] Adicionar e testar as bibliotecas de conexão WiFi e bluetooth;<br/>
[ ] Documentar o [uso das bibliotecas WiFi e bluetooth](#doc_lib);<br/>
[X] Enviar e receber mensagens (bytes) via bluetooth (visualização na saída serial);<br/>
[ ] Adicionar capacidades de atualização via WiFi;<br/>
[ ] Documentar [integração com sensor e atuador (LED)](#doc_integration);<br/>
[ ] Adicionar e testar integração com sensor e atuador;<br/>
[ ] Preparar um vídeo demonstrativo das funcionalidades;<br/>
[ ] Preprara slides de apresentação.

# <a name="doc_app"/></a>Especificação da aplicação

A aplicação consiste de um servidor BLE (Bluetooth Low Energy) sempre aguardando ("ouvindo") novas conexões até estabelecer uma. Após essa conexão ser estabelecidade pelo cliente, o servidor para de ouvir.

Enquanto o cliente estiver conectado, ele receberá notificações sobre a humidade atual coletada diretamente do sensor em intervalos de 2 segundos cada. Além disso, o cliente poderá paralelamente enviar os seguintes comandos ao módulo:

* A: liga o LED<br/>
* B: desliga o LED<br/>
* U: habilita o WiFi (desenvolvimento)<br/>
* D: desabilita o WiFi (desenvolvimento)<br/>
* R: reseta as configuração de WiFi (desenvolvimento)

Estes comandos devem ser enviados pelo terminal no aplicativo *BLE scanner* instalado, por exemplo, no celular do usuário. O comando "U" é destinado a desenvolvedores que desejem atualizar o *firmware* do módulo via WiFi utilizando WiFiManager e ArduinoOTA.

# <a name="doc_dev"/></a>Ambiente de desenvolvimento

Para configurar o ambiente de desenvolvimento deste projeto, incialmente é necessário instalar o *git* [[1](#bib_git)]. Em sistemas operacionais Linux e Mac Os recomenda-se a instalação pelo *package manager* disponível. Já no sistema operacional Windows, recomenda-se a instalação do *GitBash* [[2](#bib_shell)]. Em seguinda, clone o repositório:

```bash
git clone https://github.com/matheuskknd/bcc_internet_of_things
cd bcc_internet_of_things
```

Logo depois instale a IDE do Arduino [[3](#bib_ide)], abra o programa e permita quaisquer regras através do firewall (evita problemas futuros). Então adicione um repositório de bibliotecas adicionais da seguite forma:

1. Navegue até: Arquivo -> Preferências;
2. No campo "URLs Adicionais para Gerenciadores de Placas" copie essa URL: ```https://dl.espressif.com/dl/package_esp32_index.json``` e clique em "OK";

Em seguida, baixe todas as dependências:

1. Navegue até: Ferramentas -> Placa: "XXX" -> Gerenciador de Placas...;
2. Pesquise por "esp32", selecione a última versão e clique em "Instalar";
3. Volte a janela principal, e navegue até: Ferramentas -> Gerenciar Bibliotecas...;
4. Pesquise "tzapu" -> Instale a biblioteca WiFiManager (by tablatronix);
5. Pesquise "NimBLE" -> Instale a biblioteca NimBLE-Arduino (by h2zero);
6. Pesquise "DHT sensor" -> Instale a biblioteca DHT sensor library (by Adafruit);
7. Pesquise "Adafruit Unified Sensor" -> Instale a biblioteca Adafruit Unified Sensor (by Adafruit).

**NOTA**: para utilizar a funcionalidad do OTA pela IDE, é necessário ter o Python [[4](#bib_py)] instalado na máquina. Em sistemas operacionais Linux e Mac Os recomenda-se a instalação pelo *package manager* disponível. Já no sistema operacional Windows 10 ou superior, recomenda-se baixar a versão mais atulizada do Python 3 pela Microsoft Store.

Por fim, selecione o módulo ESP32 na IDE e comece a trabalhar:

1. Navegue até: Ferramentas -> Placa: "XXX" -> ESP32 Arduino -> ESP32 Dev Module;
2. Conecte seu ESP32 na porta USB da sua escolha (uma luz deve ficar acesa enquanto conectado);
3. Navegue até: Ferramentas -> Portas e selecione seu dispositivo;
    1. Se for a primeira vez conectando o disposivo, pode ser que o Windows não consiga detectar os drivers. Neste caso, siga o [passo a passo](#drivers) "Troubleshooting ESP32 drivers.";
    2. Caso seu dispositivo não apareça na IDE (mesmo que a luz esteja acesa), o cabo pode ser o problema. Recomenda-se testar com outro cabo. Alguns cabos não possuem fios dedicados a passagem de dados, apenas os de carga e, por isso, jamais vão funcionar para este fim.
4. Feito isso, já deve ser possível compilar e carregar o programa como *firmware* no módulo.

Ademais, basta realizar as modificações pretendidas no código fonte e utilizar o GIT para controle de versionamento.

## <a name="drivers"/></a>Troubleshooting ESP32 drivers

O Windows pode não detectar os drivers automaticamente caso seja a primeira vez que um dispositivo é conectado. Para permitir que o Windows consiga se comunicar com o módulo pode ser necessário instalar os drivers manualmente. Veja as instruções:

1. Neste site: ```https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers``` navegue até a aba "Downloads";
2. Baixe o arquivo "CP210x Universal Windows Driver" e descompacteo com "Extrair para";
3. Entre na pasta recém descompactada, clique com o botão direito do mouse no arquivo "silabser.inf" e clique em "Instalar";
4. Feito isso, deve bastar reconectar o dispositivo à porta USB para que ele apareça na IDE;
5. Os arquivos baixados e extraídos podem ser apagados, pois não serão mais necessários.

**Nota**: este *troubleshooting* funciona quando USDB to UART (não é necessário USB to TTL *standalone* nesta placa) for o CP210x, quando for o CH340 olhar este site para obter informações: ```https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all```.

# <a name="doc_lib"/></a>Dependências (bibliotecas)

**TODO**

# <a name="doc_integration"/></a>Integração com sensores e LED

**TODO**

# Bibliografia

1. <a name="bib_git"/>https://github.com</a><br/>
2. <a name="bib_shell"/>https://git-scm.com/download/win</a><br/>
3. <a name="bib_ide"/>https://www.arduino.cc/en/software</a><br/>
4. <a name="bib_py"/>https://www.python.org/downloads/</a><br/>
5. <a name="bib_diag"/>https://fritzing.org/</a><br/>
6. <a name="bib_led"/>https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink</a><br/>
7. <a name="bib_pinout"/>https://github.com/AchimPieters/esp32-homekit-camera/blob/master/Images/ESP32-30Pin-devboard.pdf</a>
8. <a name="bib_ble"/>https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide</a><br/>
9. <a name="bib_nimble"/>https://github.com/h2zero/NimBLE-Arduino</a>
10. <a name="bib_nRFCon"/>https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp</a>
11. <a name="bib_devdoc"/>https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html</a>
12. <a name="bib_uuids"/>https://www.bluetooth.com/specifications/assigned-numbers</a>
13. <a name="bib_uuidGen"/>https://www.uuidgenerator.net</a>
14. <a name="bib_dht"/>https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide</a>
