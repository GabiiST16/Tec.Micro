import matplotlib.pyplot as plt

tiempos = [0, 5, 15, 25, 35]
corrientes_mA = [28.0, 23.8, 22.5, 21.4]

plt.figure(figsize=(12, 7))
plt.step(tiempos, [corrientes_mA[0]] + corrientes_mA, where='post', linewidth=2.5)
plt.title('Consumo de Corriente en Función del Tiempo', fontsize=16)
plt.xlabel('Tiempo (segundos)', fontsize=12)
plt.ylabel('Corriente (mA)', fontsize=12)
plt.xlim(0, 35)
plt.ylim(0, max(corrientes_mA) * 1.1)
plt.xticks(range(0, 36, 5))
plt.text(2.5, 28.5, 'Modo Activo', ha='center', color='black', weight='bold')
plt.text(10, 24.3, 'Modo IDLE', ha='center', color='black', weight='bold')
plt.text(20, 23.0, 'ADC Noise', ha='center', color='black', weight='bold')
plt.text(30, 21.9, 'Power-Down', ha='center', color='black', weight='bold')
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.tight_layout()
plt.show()
