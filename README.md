# Projeto desenvolvimento de aplicação utilizando Bluetooth

Este repositório contém os arquivos e especificações do projeto final da disciplina de Internet das Coisas (IOT). O projeto escolhido foi: "Desenvolvimento de aplicação utilizando Bluetooth". As seguintes funcionalidades devem ser implementadas:

* Conexão Wi-Fi:
    * Atualização com biblioteca OTA (via Wi-Fi).
* Conexão bluetooth:
    * A conexão Wi-Fi deve ser ativada pelo bluetooth (economia de energia);
    * O usuário deve ser capaz de comandar o LED a ligar/desligar (via bluetooth);
    * Dados de umidade devem ser enviados com periodicidade (via bluetooth).
* Ler dados de umidade do sensor.

O módulo a ser utilizado será o ESP32, pois possui capacidades de conexão via Wi-Fi e bluetooth sem que seja necessária a adição de outros componentes.

## Planejamento

Segue o planejamento quanto à adiação de funcionalidades:

[X] Documentar a [especificação da aplicação](#doc_app);<br/>
[X] Documentar o [ambiente de desenvolvimento](#doc_dev);<br/>
[X] Adicionar e testar as bibliotecas de conexão Wi-Fi e bluetooth;<br/>
[X] Documentar o [uso das bibliotecas Wi-Fi e bluetooth](#doc_lib);<br/>
[X] Enviar e receber mensagens (bytes) via bluetooth (visualização na saída serial);<br/>
[X] Adicionar capacidades de atualização via Wi-Fi;<br/>
[ ] Documentar [integração com sensor e atuador (LED)](#doc_integration);<br/>
[ ] Preparar um vídeo demonstrativo das funcionalidades;<br/>
[X] Adicionar e testar integração com sensor e atuador;<br/>
[ ] Preprara slides de apresentação.

# <a name="doc_app"/></a>Especificação da aplicação

A aplicação consiste de um servidor BLE (Bluetooth Low Energy) sempre aguardando ("ouvindo") novas conexões até estabelecer uma. Após essa conexão ser estabelecidade pelo cliente, o servidor para de ouvir.

Enquanto o cliente estiver conectado, ele receberá notificações sobre a humidade atual coletada diretamente do sensor em intervalos de 2 segundos cada. Além disso, o cliente poderá paralelamente enviar os seguintes comandos ao módulo:

* A: liga o LED<br/>
* B: desliga o LED<br/>
* U: habilita o Wi-Fi e OTA (desenvolvimento)<br/>
* D: desabilita o Wi-Fi e OTA (desenvolvimento)<br/>
* R: reseta as configuração de Wi-Fi (desenvolvimento)

Estes comandos devem ser enviados pelo terminal no aplicativo *BLE scanner* instalado, por exemplo, o aplicativo de celular *nRF Connect for Mobile* [[1](#bib_nRFCon)]. O comando "U" é destinado a desenvolvedores que desejem atualizar o *firmware* do módulo via Wi-Fi utilizando WiFiManager e ArduinoOTA.

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

Neste capítulo são explicadas as dependências (*includes*) do projeto. Todas as depências são instaladas conforme explicado no capítulo anterior. A biblioteca ```ArduinoOTA.h``` vem junto com o *kit* de desenvolvimento da placa. Já ```Arduino.h``` e ```cstdint``` são bibliotecas padrão que toda instalação do compilador possuem.

## Arduino.h

Essa biblioteca padrão contém declarações e algumas definições de funções, variáveis e constantes utilizadas para desenvolmento cotidiano em microcontroladores NodeMCU. Algumas destas são: o tipo ```String```; as funções ```pinMode``` e ```digitalWrite``` etc.

**NOTAS**: essa biblioteca sempre é incluída implicitamente quando o arquivo sendo compilado tem a extensão ```.ino```. Caso possua uma extensão diferente, é necessário a inclusão explícita.

## cstdint

Essa é uma biblioteca padrão do C++ que se origina da biblioteca ```stdint.h``` da linguagem C. De acordo com a documentação [[15](#bib_cstdint)]:

> Este cabeçalho pertence originalmente à biblioteca padrão do C como ```stdint.h```.

> Este cabeçalho é parte da biblioteca de suporte à tipos, provendo tipos inteiros de tamanho fixo e partes da interface de limites numéricos da linguagem C.

## NimBLEDevice.h

Essa biblioteca provê uma implementação alternativa para as classes e funções do Bluetooth Low Energy (BLE) [[8](#bib_ble)], que é, segundo os autores [[9](#bib_nimble)]:

> Um *fork* da *stack* NimBLE refatorada para compilação na IDE Arduino [[3](#bib_ide)]<br/>
> ...

> Esta biblioteca reduz significativamente o uso de recursos computacionais e melhora a eficiência de aplicações BLE para ESP32 comparada à biblioteca base ```bluedroid```. O intuito é manter, tanto quanto seja razoável, compatibilidade com a biblioteca original, porém refatorada, para uso com a *stack* NimBLE. Além disso, esta biblioteca deve ser mais ativamente desenvolvida e mantida para prover capacidades e estabilidade melhoradas em relação à original.<br/>
> ...

De fato, os autores estão corretos em afirmar que há redução significativa de uso de recursos computacionais quando se usa sua biblioteca em relação à original (que vem junto ao *kit* de desenvolvimento da placa). Por exemplo: é impossível compilar este projeto com a biblioteca original, pois o tamanho do arquivo gerado não cabe na memória *flash* da placa reservada para programas. No entanto, utilizando esta bilioteca, ainda sobra cerca de 20% do referido espaço.

Além da redução de uso de recursos, esta biblioteca possui algumas funções auxiliares que reduzem o tamanho do código do cliente como: ```NimBLEServer::advertiseOnDisconnect``` etc. Por isso seu uso é sempre recomendado em relação à biblioteca original.

## DHT.h

Esta biblioteca forcene classes e funções utilizadas para permitir ao código cliente configurar e consultar de maneira simples a leitura de dados de sensores DHT. Apenas instanciado-se um objeto ```DHT``` passando o tipo de sensor (*hardware*) e o pino de sinal, e chamando duas outras funções já é possível ler, em tempo real, dados do sensor de humidade e temperatura DHT.

**NOTAS**: a biblioteca "Adafruit Unified Sensor" (```Adafruit_Sensor.h```) é um dependência desta, por isso não há descrições sobre ela neste documento nem inclusões no código cliente.

## ArduinoOTA.h

Esta biblioteca forcene classes e funções utilizadas para permitir ao código cliente implementar de maneira simples a funcionalidade OTA (*Over The Air*). Esta, por sua vez, permite realizar autlizações de *firmware* através de rede. No caso do ESP32, através da conexão Wi-Fi.

## WiFiManager.h

Esta biblioteca forcene classes e funções utilizadas para permitir ao código cliente implementar de maneira simples conexões sucessivas e novas conexões Wi-Fi. Ao utilizá-la, é possível configurar os SSIDs e suas respectivas senhas em tempo de execução geralmente logo após o *boot* na primeira execução, excluindo a necessidade de ter SSID e senha *hardcoded* (gravados em tempo de compilação).

# <a name="doc_impl"/></a>Implementação

Este capítulo possui uma breve descrição sobre a implementação de cada classe. Inicialmente, supõe-se que o leitor já possua alguma afinidade com organização de projetos Arduino, C++ e POO.

Todas as classes possuem métodos ```setup``` e ```loop```, que devem ser chamados quando é necessário realizar a inicialização do objeto (```setup```) e posteriormente para sua operação ao longo do tempo (```loop```). A implementação foi separada em classes e arquivos:

* ```app.ino```:
    * Arquivo principal;
    * Necessário para compilação na IDE Arduino;
    * Contém as definições das funções globais ```setup``` e ```loop```;
    * Responsável por instanciar um objeto BluetoothController e chamar seus métodos ```setup``` e ```loop```;
    * Inicializa a saída serial.

* BluetoothController:
    * Classe principal;
    * Implementa a conexão bluetooth;
    * Contrói objetos das outras classes;
    * Chama os métodos de inicialização e operação nos objetos de outras classes conforme aplicável.

* LedController:
    * Classe responsável por controlar o atuador (LED);
    * Implementa a configuração e operação (escrita) do LED sobre o pino de sinal D5.

* SensorController:
    * Classe responsável por controlar o sensor (DHT11);
    * Implementa a configuração e operação (leitura) do sensor (DHT11) sobre o pino de sinal D4.

* UpdateController:
    * Classe responsável por controlar a atualização via OTA e, consequentemente, a conexão Wi-Fi;
    * Implementa configuração e operação de conexão Wi-Fi;
    * Implementa configuração e operação de conexão OTA;
    * Mantém a conexão Wi-Fi e OTA, quando aplicável.

**NOTAS ESPECIAIS**: a utilização das macros ```F``` e ```PSTR``` se deram necessárias para reduzir o espaço que o programa ocupa na memória RAM. Pois estas macros "movem" a *string* que recebem como parâmetro para a memória *flash* da placa. A única diferença entre elas é que a ```F``` é uma composição da ```PSTR``` seguida de um *c-style cast* como: ```(__FlashStringHelper*)```. Este tipo ajuda o compilador escolher a sobrecarga de função correta para processar tais *strings*, já que elas não podem ser lidas como *strings* comuns. Por exemplo, o seguinte código é necessário para trazê-las para a memória principal:

```cpp
constexpr static auto FLASH_STR = PSTR("string armazena em flash")

...

char buffer[64];
strcpy_P((char *)&buffer, FLASH_STR);
funcao_que_espera_char_pointer((char *)&buffer);
```

## BluetoothController

Depende da biblioteca ```NimBLEDevice.h``` .

### Contrutor:

Responsável por contruir objetos das outras classes.

### Destrutor:

Responsável por destruir explicitamente os objetos alocados dinamicamente (com operador ```new```).

### setup:

Responsável por:

1. Inicializar os objetos das classes SensorController e LedController, respectivamente;
2. Inicializar o dispositivo BLE;
3. Criar um servidor BLE;
4. Criar um serviço BLE (com UUID: "8bfb95e0-412a-4a9a-985b-7e291c51f459" [[13](#bib_uuidGen)]);
5. Cria a característica BLE "sensor de humidade" (UUID número: 10863 (Humidity) [[12](#bib_uuids)]), com capacidades de leitura e notificação;
6. Adicionar um descritor BLE à característica "sensor de humidade" (UUID número: 10500 (*Characteristic Presentation Format*) [[12](#bib_uuids)]), descrevendo a variável de 4 bytes com formato ponto fluante IEEE-754 32-bit e percentual;
7. Cria a característica BLE "linha de comando" (UUID número: 11046 (IDD Command Data) [[12](#bib_uuids)]), com capacidade de escrita apenas;
8. Configurar os callbacks utilizados para processar os comandos escritos na característica "linha de comando";
9. Inicializar o serviço BLE;
10. Configurar e inicializa a propagação o serviço (advertisement).

### loop:

Responsável por:

1. Chamar o método ```loop``` nos objetos das classes UpdateController, SensorController e LedController, respectivamente;
2. Gerenciar o estado de conexão de possíveis clientes;
3. Notificar o cliente, caso conectado, sobre alterações siginficativas na humidade percentual (ao menos 0.5% de diferença).

## LedController

Depende da biblioteca ```Arduino.h``` .

### Contrutor:

Responsável por configurar o pino de sinal do atuador (LED), neste caso: D5.

### Destrutor:

Não faz nada.

### setup:

Responsável por:

1. Configurar o pino de sinal D5 como saída;
2. Configurar o estado do pino como ```LOW``` (desativado).

### loop:

Não faz nada.

### set:

Responsável por configurar o estado do pino como ```LOW``` (desativado) ou ```HIGH``` (ativado) dependendo do parâmetro booleano.

## SensorController

Depende da biblioteca ```DHT.h``` .

### Contrutor:

Responsável por contruir o objeto da classe ```DHT``` passando o pino de sinal como D4 e o tipo do sensor com DHT11.

### Destrutor:

Responsável por destruir explicitamente os objetos alocados dinamicamente (com operador ```new```).

### setup:

Responsável por inicializar a operação do objeto ```DHT```.

### loop:

Responsável por:

1. Tentar ler dados do sensor a cada 2 segundos;
2. Caso leia os dados com sucesso, atualizar o estado interno.

### value:

Retorna o estado interno: última leitura com sucesso dos dados de humidade do sensor.

## UpdateController

Depende das bibliotecas ```ArduinoOTA.h``` e ```WiFiManager.h```.

### Contrutor:

Responsável por:

1. Contruir os da classe ```WiFiManager``` e ```WiFiClient```;
2. Dar um nome único ao dispositivo para reconhecemo-lo na rede.

### Destrutor:

Responsável por destruir explicitamente os objetos alocados dinamicamente (com operador ```new```).

### setup:

Responsável por:

1. Configurar o *hostname* (nome do host na rede) com o ID único;
2. Chamar a função ```initWiFi``` (da própria classe).
3. Chamar a função ```initOTA``` (da própria classe).
4. Configurar o estado interno como habilitado.

### loop:

Responsável por:

1. Não fazer nada caso o estado interno esteja em "dormindo" ou desabilitado;
2. Se naõ, garantir que o Wi-Fi esteja conectado chamando a função ```ensureWiFi``` (da própria classe);
3. Caso o Wi-Fi esteja conectado, faz o *keep-alive* da conexão OTA.

### tearDown:

Responsável por:

1. Desabilitar a conexão OTA e desfazer toda sua configuração;
2. Desconectar a conexão Wi-Fi;
3. Configurar o estado interno como desabilitado.

### resetSettings:

Responsável por deletar todos os SSIDs e senhas de redes Wi-Fi conhecidas.

### copyId:

Retorna uma cópia do nome único do dispositivo.

### initWiFi (protected):

Responsável por:

1. Chamar o método ```WiFiManager::autoConnect```, que conecta a uma rede já conhecida; ou
2. Faz um *fallback* em que o dispositivo se tona um AP (*Access Point*) com uma página Web de configuração para se conectar alguma rede Wi-Fi.

### initOTA (protected):

Responsável por:

1. Configurar o *hostname* OTA como o nome único do dispositivo;
2. Configurar a senha de conexão OTA (*hardcoded*);
3. Configurar os callbacks chamados nos diversos eventos do OTA;
4. Habilita a conexão OTA no dispositivo.

### ensureWiFi (protected):

Responsável por:

1. Verificar o estado da conexão Wi-Fi; e
2. Caso esteja conectado, apenas retorna verdadeiro;
3. Caso não esteja conectado, faz o seguinte:
    1. Caso o contador não esteja inicializado, inicializa-o em ```MAX_ATTEMPT_CONNECTION + 1```;
    2. Caso o contador já esteja inicializado e não seja ```1```, apenas decrementa-o em 1;
    3. Caso o contador esteja em 1, chama a função ```initWiFi``` (da própria classe) configura o contador em 0;
    4. Configura o estado interno como "dormindo" por 3 segundos.

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
15. <a name="bib_cstdint"/>https://en.cppreference.com/w/cpp/header/cstdint</a>
