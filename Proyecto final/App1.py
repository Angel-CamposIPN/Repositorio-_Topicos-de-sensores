import flet as ft
import serial
import threading
import json

PUERTO = "COM7"
BAUDIOS = 115200


def main(page: ft.Page):

    page.title = "ChalecoBIO Monitor"
    page.theme_mode = ft.ThemeMode.DARK
    page.window_width = 900
    page.window_height = 650

    temperatura = ft.Text("0 °C", size=40, weight=ft.FontWeight.BOLD)
    humedad = ft.Text("0 %", size=40, weight=ft.FontWeight.BOLD)
    presion = ft.Text("0", size=30)
    altitud = ft.Text("0", size=30)

    estado = ft.Text(
        "Esperando datos...",
        size=24,
        weight=ft.FontWeight.BOLD
    )

    recomendacion = ft.Text(
        "Sin datos",
        size=18
    )

    def evaluar(temp, hum):

        if temp > 35:
            return (
                "🔴 CALOR ELEVADO",
                "Mantente hidratado y evita exposición prolongada al sol."
            )

        if temp < 15:
            return (
                "🔵 FRÍO",
                "Usa ropa abrigadora."
            )

        if hum > 80:
            return (
                "🟡 MUY HÚMEDO",
                "Puede existir sensación térmica elevada."
            )

        return (
            "🟢 CONFORTABLE",
            "Condiciones ambientales adecuadas."
        )

    def leer_serial():

        try:

            print("Abriendo puerto...")

            ser = serial.Serial(
                PUERTO,
                BAUDIOS,
                timeout=1
            )

            ser.reset_input_buffer()

            print("Puerto abierto correctamente")

            while True:

                try:

                    linea = (
                        ser.readline()
                        .decode(errors="ignore")
                        .strip()
                    )

                    if not linea:
                        continue

                    print("RECIBIDO:", linea)

                    if not linea.startswith("{"):
                        continue

                    datos = json.loads(linea)

                    temp = float(datos["temp"])
                    hum = float(datos["humedad"])
                    pres = float(datos["presion"])
                    alt = float(datos["altitud"])

                    est, reco = evaluar(temp, hum)

                    temperatura.value = f"{temp:.1f} °C"
                    humedad.value = f"{hum:.1f} %"
                    presion.value = f"{pres:.2f}"
                    altitud.value = f"{alt:.1f} m"

                    estado.value = est
                    recomendacion.value = reco

                    page.update()

                except Exception as e:
                    print("ERROR:", e)

        except Exception as e:

            estado.value = f"Error serial: {e}"
            page.update()

            print("ERROR SERIAL:", e)

    page.add(
        ft.Column(
            [
                ft.Text(
                    "CHALECOBIO",
                    size=35,
                    weight=ft.FontWeight.BOLD
                ),

                ft.Divider(),

                ft.Row(
                    [
                        ft.Container(
                            content=ft.Column(
                                [
                                    ft.Text("🌡 Temperatura"),
                                    temperatura
                                ]
                            ),
                            padding=20
                        ),

                        ft.Container(
                            content=ft.Column(
                                [
                                    ft.Text("💧 Humedad"),
                                    humedad
                                ]
                            ),
                            padding=20
                        ),
                    ]
                ),

                ft.Row(
                    [
                        ft.Container(
                            content=ft.Column(
                                [
                                    ft.Text("🧭 Presión"),
                                    presion
                                ]
                            ),
                            padding=20
                        ),

                        ft.Container(
                            content=ft.Column(
                                [
                                    ft.Text("⛰ Altitud"),
                                    altitud
                                ]
                            ),
                            padding=20
                        ),
                    ]
                ),

                ft.Divider(),

                estado,

                ft.Text(
                    "Recomendación:",
                    size=20,
                    weight=ft.FontWeight.BOLD
                ),

                recomendacion
            ]
        )
    )

    hilo = threading.Thread(
        target=leer_serial,
        daemon=True
    )

    hilo.start()


ft.app(target=main)