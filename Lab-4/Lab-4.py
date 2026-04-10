import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import savgol_filter

def harmonic_with_noise(amplitude, frequency, phase, noise_mean, noise_covariance, show_noise):
    t = np.linspace(0, 10, 1000)
    harmonic = amplitude * np.sin(2 * np.pi * frequency * t + phase)
    
    global current_noise_values
    
    if show_noise:
        y_result = harmonic + current_noise_values
    else:
        y_result = harmonic
        
    return t, y_result, harmonic

init_amp = 1.0
init_freq = 0.25
init_phase = 0.0
init_mean = 0.0
init_cov = 0.1
init_window = 51

t_space = np.linspace(0, 10, 1000)
current_noise_values = np.random.normal(init_mean, np.sqrt(init_cov), len(t_space))

fig, ax = plt.subplots(figsize=(10, 7))
plt.subplots_adjust(left=0.1, bottom=0.35, right=0.9, top=0.9)


t, y_signal, y_pure = harmonic_with_noise(init_amp, init_freq, init_phase, init_mean, init_cov, True)

[line_noisy] = ax.plot(t, y_signal, label='Зашумлена', color='#ffa500', alpha=0.8, lw=1.5)

[line_pure] = ax.plot(t, y_pure, '--', label='Еталон (чиста)', color='#0000ff', lw=1.5)

window = init_window if init_window % 2 != 0 else init_window + 1
y_filt = savgol_filter(y_pure + current_noise_values, window, 3)

[line_filtered] = ax.plot(t, y_filt, label='Відфільтрована', color='#800080', lw=2)

ax.set_title("Порівняння сигналів", fontsize=12)
ax.legend(loc='upper right', frameon=True)
ax.grid(True, alpha=0.3)
ax.set_ylim(-2.2, 2.2)

ax_color = '#f0f0f0'
widget_width = 0.4

ax_amp = plt.axes([0.25, 0.23, widget_width, 0.03], facecolor=ax_color)
ax_freq = plt.axes([0.25, 0.19, widget_width, 0.03], facecolor=ax_color)
ax_phase = plt.axes([0.25, 0.15, widget_width, 0.03], facecolor=ax_color)

ax_mean = plt.axes([0.25, 0.11, widget_width, 0.03], facecolor=ax_color)
ax_cov = plt.axes([0.25, 0.07, widget_width, 0.03], facecolor=ax_color)
ax_window = plt.axes([0.25, 0.03, widget_width, 0.03], facecolor=ax_color) # Слайдер фільтра

s_amp = Slider(ax_amp, 'Амплітуда (A)', 0.1, 2.0, valinit=init_amp)
s_freq = Slider(ax_freq, 'Частота (f)', 0.05, 1.0, valinit=init_freq)
s_phase = Slider(ax_phase, 'Фаза (φ)', 0, 2*np.pi, valinit=init_phase)
s_mean = Slider(ax_mean, 'Шум (M)', -1.0, 1.0, valinit=init_mean)
s_cov = Slider(ax_cov, 'Шум (D)', 0.001, 0.5, valinit=init_cov)

s_window = Slider(ax_window, 'Вікно фільтра', 5, 151, valinit=init_window, valfmt='%0.0f')

ax_reset = plt.axes([0.1, 0.02, 0.08, 0.04])
btn_reset = Button(ax_reset, 'Reset', color='#ffcccc')

ax_check = plt.axes([0.8, 0.02, 0.15, 0.04], facecolor='#e0e0e0')
check = CheckButtons(ax_check, ['Show Noise'], [True])

def update(val):
    global current_noise_values
    
    if val == s_mean.val or val == s_cov.val:
        current_noise_values = np.random.normal(s_mean.val, np.sqrt(s_cov.val), len(t_space))

    show_noise_flag = check.get_status()[0]
    t, y_to_show, y_pure_new = harmonic_with_noise(
        s_amp.val, s_freq.val, s_phase.val, s_mean.val, s_cov.val, True # Завжди отримуємо зашумлений для фільтра
    )
    
    line_noisy.set_ydata(y_pure_new + current_noise_values)
    line_pure.set_ydata(y_pure_new)
    
    line_noisy.set_visible(show_noise_flag)
    
    raw_window = int(s_window.val)
    current_window = raw_window if raw_window % 2 != 0 else raw_window + 1
    
    y_for_filter = y_pure_new + current_noise_values
    y_filt_new = savgol_filter(y_for_filter, current_window, 3)
    line_filtered.set_ydata(y_filt_new)
    
    fig.canvas.draw_idle()

for s in [s_amp, s_freq, s_phase, s_mean, s_cov, s_window]:
    s.on_changed(update)
check.on_clicked(update)

def reset_all(event):
    s_amp.reset()
    s_freq.reset()
    s_phase.reset()
    s_mean.reset()
    s_cov.reset()
    s_window.reset()
btn_reset.on_clicked(reset_all)

plt.show()