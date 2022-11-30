//pines de entrada

const int refPin = A0;
const int salPin = A1;
//const int stby   = 6;
const int aPin   = 5;
const int bPin   = 4;
const int pwmPin = 3;

//ganancias 
const float beta = 0.1;
const float Gamma = 0.5; //Red neuronal
const float Lambda = 40.0; //proporcional
const float Kd = 10.0; //Derivada

const int n = 4; // numero de neuronas

const float Dn [][3] = {{-0.1, 0.1,   0.0},
                        {0.1,  0.0,   0.05},
                        {0  ,  0.05, -0.1},
                        {0.05, -0.1,  0.1}};

float W[]= {0, 0, 0, 0, 0}; //pesos de la red
float y_ = 0; //Retroalimentación anterior
float yd_ = 0; // Referencia anterior

unsigned long t_; //tiempo anterior 

void setup() {
  
  Serial.begin(115200); // velocidad de la comunicación serial
//definimos los pines como salidas

pinMode(pwmPin, OUTPUT);
pinMode(aPin, OUTPUT);
pinMode(bPin, OUTPUT);
//pinMode(stby, OUTPUT);

t_ = millis(); // tiempo en milis
delay(100);

}

void loop() {
  unsigned long t = millis();// variable del tiempo en 32 bits y milisegundos
  float dt = (t - t_)/1000.0; // incremento del tiempo
  //float yd = 2.0*sin(2.0*3.1416*millis()/(1000*5.0))+2.4; // referencia senosoidal
  float yd = analogRead(refPin)*5.0/1023.0;
  float yd_p = (yd - yd_)/dt;
  float y = analogRead(salPin)*5.0/1023.0;
  float y_p = (y - y_)/dt; 

// osciloscopio
  Serial.print(yd);
  Serial.print(", ");
  Serial.println(y);

  float u = control_nn(y, yd, y_p, yd_p); // función para optener u
  actuador(u);

// variables para nuevo ciclo

  y_ = y;
  yd_ = yd; 
  t_  = t;

}

float control_nn(float y, float yd, float y_p, float yd_p){
  float e   = yd - y;
  float e_p = yd_p - y_p;
  float s  = Lambda*e+e_p;
  float v   = Lambda*e+yd_p;

  float Xn[]= {v,y,1}; // Entradas de la red
  float S[n], f = 0; 

  for(int k = 0; k < n; k++){
    S[k] = sigmoidal(Dn[k][0]*Xn[0] + Dn[k][1]*Xn[1] + Dn[k][2]*Xn[2]); // se multiplican las entradas de la red por las columnas Dn
    W[k] = W[k] + Gamma*S[k]*s; // regla de aprendizaje
    f = f + W[k]*S[k]; // salida de la red
    
   }
    return (f + Kd*s); // se obtiene el valor de u
      
}
float sigmoidal( float u) {
   return (2.0/(1.0+pow(2.7103,-beta*u))-1.0);
  
  }

void actuador(float u){
  analogWrite(pwmPin, (int)abs(u)); //valor absoluto de u como PWM

  if(u>0){
    //digitalWrite(stby,HIGH);
    digitalWrite(aPin,HIGH);
    digitalWrite(bPin,LOW);
    
   }
  else{
    //digitalWrite(stby,HIGH);
    digitalWrite(bPin,HIGH);
    digitalWrite(aPin,LOW);
     }
  }
                        
