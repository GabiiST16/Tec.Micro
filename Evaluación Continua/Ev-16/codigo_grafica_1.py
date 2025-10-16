import matplotlib.pyplot as plt

modos = ['Activo (On)', 'IDLE', 'ADC Noise', 'Power-Down']
corrientes_mA = [28.0, 23.8, 22.5, 21.4]

plt.figure(figsize=(10, 6))
bars = plt.bar(modos, corrientes_mA, color=['#ff6347', '#4682b4', '#32cd32', '#6a5acd'])
plt.title('Consumo de Corriente por Modo de Operación', fontsize=16)
plt.ylabel('Corriente (mA)', fontsize=12)
plt.xlabel('Modo del Microcontrolador', fontsize=12)
plt.ylim(0, max(corrientes_mA) * 1.2)
for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width() / 2, yval + 0.3, f'{yval:.1f}', ha='center', va='bottom', fontsize=10)
plt.tight_layout()
plt.show()
