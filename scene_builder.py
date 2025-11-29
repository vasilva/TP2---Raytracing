#!/usr/bin/env python3
"""
Scene Builder for Raytracer
Interactive tool to create scene files for the raytracer program.
"""

import sys
import os
import argparse


class Camera:
    def __init__(
        self, position=(0, 30, -200), target=(0, 10, -100), normal=(0, 1, 0), fov=40
    ):
        self.position = position
        self.target = target
        self.normal = normal
        self.fov = fov

    def to_string(self):
        return (
            f"{self.position[0]} {self.position[1]} {self.position[2]}\n"
            f"{self.target[0]} {self.target[1]} {self.target[2]}\n"
            f"{self.normal[0]} {self.normal[1]} {self.normal[2]}\n"
            f"{self.fov}\n"
        )


class Light:
    def __init__(self, position=(0, 0, 0), color=(1, 1, 1), attenuation=(1, 0, 0)):
        self.position = position
        self.color = color
        self.attenuation = attenuation

    def to_string(self):
        return (
            f"{self.position[0]} {self.position[1]} {self.position[2]}\t"
            f"{self.color[0]} {self.color[1]} {self.color[2]}\t"
            f"{self.attenuation[0]} {self.attenuation[1]} {self.attenuation[2]}\n"
        )


class Pigment:
    def __init__(self, ptype, **kwargs):
        self.type = ptype  # 'solid', 'checker', 'texmap'
        self.params = kwargs

    def to_string(self):
        if self.type == "solid":
            c = self.params.get("color", (1, 1, 1))
            return f"solid\t{c[0]} {c[1]} {c[2]}\n"

        elif self.type == "checker":
            c1 = self.params.get("color1", (0, 0, 0))
            c2 = self.params.get("color2", (1, 1, 1))
            size = self.params.get("size", 40)
            return (
                f"checker\t{c1[0]} {c1[1]} {c1[2]}\t\t"
                f"{c2[0]} {c2[1]} {c2[2]}\t\t{size} \n"
            )

        elif self.type == "texmap":
            filename = self.params.get("filename", "texture.ppm")
            p0 = self.params.get("p0", (0, 0.001, 0, 0.12))
            p1 = self.params.get("p1", (0, 0, 0, 0))
            return (
                f"texmap\t{filename}\n"
                f"{p0[0]} {p0[1]} {p0[2]} {p0[3]}\n"
                f"{p1[0]} {p1[1]} {p1[2]} {p1[3]}\n"
            )


class SurfaceFinish:
    def __init__(self, ka=0.3, kd=0.4, ks=0.0, alpha=1, kr=0.0, kt=0.0, ior=0.0):
        self.ka = ka
        self.kd = kd
        self.ks = ks
        self.alpha = alpha
        self.kr = kr
        self.kt = kt
        self.ior = ior

    def to_string(self):
        return (
            f"{self.ka:.2f} {self.kd:.2f} {self.ks:.2f}\t{self.alpha}\t"
            f"{self.kr:.1f} {self.kt} {self.ior}\n"
        )


class Sphere:
    def __init__(self, center=(0, 0, 0), radius=10, pigment_id=0, finish_id=0):
        self.center = center
        self.radius = radius
        self.pigment_id = pigment_id
        self.finish_id = finish_id

    def to_string(self):
        return (
            f"{self.pigment_id} {self.finish_id} sphere\t\t"
            f"{self.center[0]}\t{self.center[1]}\t{self.center[2]}\t\t"
            f"{self.radius} \n"
        )


class Polyhedron:
    def __init__(self, planes, pigment_id=0, finish_id=0):
        self.planes = planes  # List of (nx, ny, nz, d) tuples
        self.pigment_id = pigment_id
        self.finish_id = finish_id

    def to_string(self):
        result = f"{self.pigment_id} {self.finish_id} polyhedron {len(self.planes)}\n"
        for plane in self.planes:
            result += f"{plane[0]}\t{plane[1]}\t{plane[2]}\t{plane[3]}\n"
        return result

    @staticmethod
    def create_cube(center=(0, 0, 0), size=10, pigment_id=0, finish_id=0):
        """Create a cube centered at position with given size"""
        half = size / 2
        cx, cy, cz = center
        planes = [
            (0, 1, 0, -(cy + half)),  # Top
            (0, -1, 0, cy - half),  # Bottom
            (1, 0, 0, -(cx + half)),  # Right
            (-1, 0, 0, cx - half),  # Left
            (0, 0, 1, -(cz + half)),  # Front
            (0, 0, -1, cz - half),  # Back
        ]
        return Polyhedron(planes, pigment_id, finish_id)

    @staticmethod
    def create_prism(
        center=(0, 0, 0), radius=10, height=10, sides=4, pigment_id=0, finish_id=0
    ):
        """Create a prism with regular polygon base

        Args:
            center: Center position (x, y, z)
            radius: Radius of the base polygon (distance from center to vertex)
            height: Height of the prism
            sides: Number of sides of the base polygon (3=triangle, 4=square, 5=pentagon, etc.)
            pigment_id: Pigment index
            finish_id: Finish index
        """
        import math

        cx, cy, cz = center
        h = height / 2

        planes = [
            (0, 1, 0, -(cy + h)),  # Top
            (0, -1, 0, cy - h),  # Bottom
        ]

        # Add side faces for regular polygon
        for i in range(sides):
            angle = 2 * math.pi * i / sides
            # Normal points outward from center
            nx = math.cos(angle)
            nz = math.sin(angle)

            # Calculate d for plane equation: nx*x + nz*z + d = 0
            # The plane passes through point at (cx + radius*nx, cy, cz + radius*nz)
            d = -(nx * (cx + radius * nx) + nz * (cz + radius * nz))

            planes.append((nx, 0, nz, d))

        return Polyhedron(planes, pigment_id, finish_id)

    @staticmethod
    def create_rectangular_prism(
        center=(0, 0, 0), width=10, height=10, depth=10, pigment_id=0, finish_id=0
    ):
        """Create a rectangular prism (box) with different dimensions"""
        w, h, d = width / 2, height / 2, depth / 2
        cx, cy, cz = center
        planes = [
            (0, 1, 0, -(cy + h)),  # Top
            (0, -1, 0, cy - h),  # Bottom
            (1, 0, 0, -(cx + w)),  # Right
            (-1, 0, 0, cx - w),  # Left
            (0, 0, 1, -(cz + d)),  # Front
            (0, 0, -1, cz - d),  # Back
        ]
        return Polyhedron(planes, pigment_id, finish_id)

    @staticmethod
    def create_pyramid(
        base_center=(0, 0, 0), radius=10, height=10, sides=4, pigment_id=0, finish_id=0
    ):
        """Create a pyramid with regular polygon base

        Args:
            base_center: Center of the base (x, y, z)
            radius: Radius of the base polygon (distance from center to vertex)
            height: Height from base to apex
            sides: Number of sides of the base polygon (3=triangle, 4=square, 5=pentagon, etc.)
            pigment_id: Pigment index
            finish_id: Finish index
        """
        import math

        cx, cy, cz = base_center

        # Base plane (bottom)
        planes = [(0, -1, 0, cy)]

        # Apex position
        apex = (cx, cy + height, cz)

        # Create triangular faces for each edge of the base polygon
        for i in range(sides):
            # Two consecutive vertices of the base polygon
            angle1 = 2 * math.pi * i / sides
            angle2 = 2 * math.pi * (i + 1) / sides

            v1x = cx + radius * math.cos(angle1)
            v1z = cz + radius * math.sin(angle1)
            v2x = cx + radius * math.cos(angle2)
            v2z = cz + radius * math.sin(angle2)

            # Three points of the triangular face: v1, v2, apex
            # v1 = (v1x, cy, v1z)
            # v2 = (v2x, cy, v2z)
            # apex = (cx, cy + height, cz)

            # Calculate normal using cross product
            # edge1 = v2 - v1
            edge1 = (v2x - v1x, 0, v2z - v1z)
            # edge2 = apex - v1
            edge2 = (cx - v1x, height, cz - v1z)

            # Normal = edge1 × edge2
            nx = edge1[1] * edge2[2] - edge1[2] * edge2[1]
            ny = edge1[2] * edge2[0] - edge1[0] * edge2[2]
            nz = edge1[0] * edge2[1] - edge1[1] * edge2[0]

            # Normalize
            length = math.sqrt(nx * nx + ny * ny + nz * nz)
            if length > 0:
                nx /= length
                ny /= length
                nz /= length

            # Calculate d using point v1 on the plane
            d = -(nx * v1x + ny * cy + nz * v1z)

            planes.append((nx, ny, nz, d))

        return Polyhedron(planes, pigment_id, finish_id)


class Scene:
    def __init__(self):
        self.camera = Camera()
        self.lights = []
        self.pigments = []
        self.finishes = []
        self.objects = []

    def add_light(self, light):
        self.lights.append(light)

    def add_pigment(self, pigment):
        self.pigments.append(pigment)
        return len(self.pigments) - 1  # Return index

    def add_finish(self, finish):
        self.finishes.append(finish)
        return len(self.finishes) - 1  # Return index

    def add_object(self, obj):
        self.objects.append(obj)

    def save(self, filename):
        """Save scene to file"""
        with open(filename, "w") as f:
            # Camera
            f.write(self.camera.to_string())

            # Lights
            f.write(f"{len(self.lights)}\n")
            for light in self.lights:
                f.write(light.to_string())

            # Pigments
            f.write(f"{len(self.pigments)}\n")
            for pigment in self.pigments:
                f.write(pigment.to_string())

            # Surface Finishes
            f.write(f"{len(self.finishes)}\n")
            for finish in self.finishes:
                f.write(finish.to_string())

            # Objects
            f.write(f"{len(self.objects)}\n")
            for obj in self.objects:
                f.write(obj.to_string())

        print(f"Scene saved to {filename}")


def interactive_mode():
    """Run interactive scene builder"""
    scene = Scene()
    print("=== Scene Builder ===")
    print("Commands:")
    print("  camera <px> <py> <pz> <tx> <ty> <tz> <nx> <ny> <nz> <fov>")
    print("  light <x> <y> <z> [r g b] [rho0 rho1 rho2]")
    print("  solid <r> <g> <b>")
    print("  checker <r1> <g1> <b1> <r2> <g2> <b2> <size>")
    print("  texmap <filename>")
    print("  finish <ka> <kd> <ks> <alpha> <kr> <kt> <ior>")
    print("  sphere <x> <y> <z> <radius> <pigment_id> <finish_id>")
    print("  cube <x> <y> <z> <size> <pigment_id> <finish_id>")
    print("  box <x> <y> <z> <width> <height> <depth> <pigment_id> <finish_id>")
    print("  prism <x> <y> <z> <radius> <height> <sides> <pigment_id> <finish_id>")
    print("    sides: 3=triangle, 4=square, 5=pentagon, 6=hexagon, 8=octagon, etc.")
    print("  pyramid <x> <y> <z> <radius> <height> <sides> <pigment_id> <finish_id>")
    print("    sides: 3=triangle, 4=square, 5=pentagon, 6=hexagon, 8=octagon, etc.")
    print("  list - Show current scene elements")
    print("  save <filename> - Save scene")
    print("  quit - Exit")
    print()

    while True:
        try:
            cmd = input("> ").strip()
            if not cmd:
                continue

            parts = cmd.split()
            command = parts[0].lower()

            if command == "quit" or command == "exit":
                break

            elif command == "camera":
                if len(parts) >= 11:
                    scene.camera = Camera(
                        position=(float(parts[1]), float(parts[2]), float(parts[3])),
                        target=(float(parts[4]), float(parts[5]), float(parts[6])),
                        normal=(float(parts[7]), float(parts[8]), float(parts[9])),
                        fov=float(parts[10]),
                    )
                    print("Camera set")
                else:
                    print(
                        "Usage: camera <px> <py> <pz> <tx> <ty> <tz> <nx> <ny> <nz> <fov>"
                    )

            elif command == "light":
                if len(parts) >= 4:
                    pos = (float(parts[1]), float(parts[2]), float(parts[3]))
                    color = (
                        (float(parts[4]), float(parts[5]), float(parts[6]))
                        if len(parts) >= 7
                        else (1, 1, 1)
                    )
                    atten = (
                        (float(parts[7]), float(parts[8]), float(parts[9]))
                        if len(parts) >= 10
                        else (1, 0, 0)
                    )
                    scene.add_light(Light(pos, color, atten))
                    print(f"Light added (total: {len(scene.lights)})")
                else:
                    print("Usage: light <x> <y> <z> [r g b] [rho0 rho1 rho2]")

            elif command == "solid":
                if len(parts) >= 4:
                    color = (float(parts[1]), float(parts[2]), float(parts[3]))
                    idx = scene.add_pigment(Pigment("solid", color=color))
                    print(f"Solid pigment added (id: {idx})")
                else:
                    print("Usage: solid <r> <g> <b>")

            elif command == "checker":
                if len(parts) >= 8:
                    c1 = (float(parts[1]), float(parts[2]), float(parts[3]))
                    c2 = (float(parts[4]), float(parts[5]), float(parts[6]))
                    size = float(parts[7])
                    idx = scene.add_pigment(
                        Pigment("checker", color1=c1, color2=c2, size=size)
                    )
                    print(f"Checker pigment added (id: {idx})")
                else:
                    print("Usage: checker <r1> <g1> <b1> <r2> <g2> <b2> <size>")

            elif command == "texmap":
                if len(parts) >= 2:
                    idx = scene.add_pigment(Pigment("texmap", filename=parts[1]))
                    print(f"Texture map pigment added (id: {idx})")
                else:
                    print("Usage: texmap <filename>")

            elif command == "finish":
                if len(parts) >= 8:
                    idx = scene.add_finish(
                        SurfaceFinish(
                            ka=float(parts[1]),
                            kd=float(parts[2]),
                            ks=float(parts[3]),
                            alpha=float(parts[4]),
                            kr=float(parts[5]),
                            kt=float(parts[6]),
                            ior=float(parts[7]),
                        )
                    )
                    print(f"Surface finish added (id: {idx})")
                else:
                    print("Usage: finish <ka> <kd> <ks> <alpha> <kr> <kt> <ior>")

            elif command == "sphere":
                if len(parts) >= 7:
                    scene.add_object(
                        Sphere(
                            center=(float(parts[1]), float(parts[2]), float(parts[3])),
                            radius=float(parts[4]),
                            pigment_id=int(parts[5]),
                            finish_id=int(parts[6]),
                        )
                    )
                    print(f"Sphere added (total objects: {len(scene.objects)})")
                else:
                    print("Usage: sphere <x> <y> <z> <radius> <pigment_id> <finish_id>")

            elif command == "cube":
                if len(parts) >= 7:
                    scene.add_object(
                        Polyhedron.create_cube(
                            center=(float(parts[1]), float(parts[2]), float(parts[3])),
                            size=float(parts[4]),
                            pigment_id=int(parts[5]),
                            finish_id=int(parts[6]),
                        )
                    )
                    print(f"Cube added (total objects: {len(scene.objects)})")
                else:
                    print("Usage: cube <x> <y> <z> <size> <pigment_id> <finish_id>")

            elif command == "box":
                if len(parts) >= 9:
                    scene.add_object(
                        Polyhedron.create_rectangular_prism(
                            center=(float(parts[1]), float(parts[2]), float(parts[3])),
                            width=float(parts[4]),
                            height=float(parts[5]),
                            depth=float(parts[6]),
                            pigment_id=int(parts[7]),
                            finish_id=int(parts[8]),
                        )
                    )
                    print(
                        f"Rectangular box added (total objects: {len(scene.objects)})"
                    )
                else:
                    print(
                        "Usage: box <x> <y> <z> <width> <height> <depth> <pigment_id> <finish_id>"
                    )

            elif command == "prism":
                if len(parts) >= 9:
                    scene.add_object(
                        Polyhedron.create_prism(
                            center=(float(parts[1]), float(parts[2]), float(parts[3])),
                            radius=float(parts[4]),
                            height=float(parts[5]),
                            sides=int(parts[6]),
                            pigment_id=int(parts[7]),
                            finish_id=int(parts[8]),
                        )
                    )
                    sides = int(parts[6])
                    shape_name = {
                        3: "triangular",
                        4: "square",
                        5: "pentagonal",
                        6: "hexagonal",
                        8: "octagonal",
                    }.get(sides, f"{sides}-sided")
                    print(
                        f"{shape_name.capitalize()} prism added (total objects: {len(scene.objects)})"
                    )
                else:
                    print(
                        "Usage: prism <x> <y> <z> <radius> <height> <sides> <pigment_id> <finish_id>"
                    )
                    print(
                        "  sides: 3=triangle, 4=square, 5=pentagon, 6=hexagon, 8=octagon, etc."
                    )

            elif command == "pyramid":
                if len(parts) >= 9:
                    scene.add_object(
                        Polyhedron.create_pyramid(
                            base_center=(
                                float(parts[1]),
                                float(parts[2]),
                                float(parts[3]),
                            ),
                            radius=float(parts[4]),
                            height=float(parts[5]),
                            sides=int(parts[6]),
                            pigment_id=int(parts[7]),
                            finish_id=int(parts[8]),
                        )
                    )
                    sides = int(parts[6])
                    shape_name = {
                        3: "triangular",
                        4: "square",
                        5: "pentagonal",
                        6: "hexagonal",
                        8: "octagonal",
                    }.get(sides, f"{sides}-sided")
                    print(
                        f"{shape_name.capitalize()} pyramid added (total objects: {len(scene.objects)})"
                    )
                else:
                    print(
                        "Usage: pyramid <x> <y> <z> <radius> <height> <sides> <pigment_id> <finish_id>"
                    )
                    print(
                        "  sides: 3=triangle (tetrahedron), 4=square, 5=pentagon, 6=hexagon, 8=octagon, etc."
                    )

            elif command == "list":
                print(f"\nCurrent Scene:")
                print(
                    f"  Camera: pos={scene.camera.position}, target={scene.camera.target}, fov={scene.camera.fov}"
                )
                print(f"  Lights: {len(scene.lights)}")
                print(f"  Pigments: {len(scene.pigments)}")
                print(f"  Finishes: {len(scene.finishes)}")
                print(f"  Objects: {len(scene.objects)}")
                print()

            elif command == "save":
                if len(parts) >= 2:
                    filename = parts[1]
                    if not filename.endswith(".txt"):
                        filename += ".txt"
                    # Save to data/scenes/ directory
                    filepath = os.path.join("data", "scenes", filename)
                    scene.save(filepath)
                else:
                    print("Usage: save <filename>")

            else:
                print(f"Unknown command: {command}")

        except KeyboardInterrupt:
            print("\nExiting...")
            break
        except Exception as e:
            print(f"Error: {e}")


def create_example_scene():
    """Create an example scene demonstrating all features"""
    scene = Scene()

    # Camera - adjusted to look at objects in front
    scene.camera = Camera(
        position=(0, 50, -120), target=(0, 15, 0), normal=(0, 1, 0), fov=45
    )

    # Lights
    scene.add_light(Light(position=(100, 100, -50), color=(1, 1, 1)))
    scene.add_light(Light(position=(-100, 100, -50), color=(0.8, 0.8, 1)))

    # Pigments
    red_pigment = scene.add_pigment(Pigment("solid", color=(1, 0.2, 0.2)))
    blue_pigment = scene.add_pigment(Pigment("solid", color=(0.2, 0.2, 1)))
    green_pigment = scene.add_pigment(Pigment("solid", color=(0.2, 1, 0.2)))
    yellow_pigment = scene.add_pigment(Pigment("solid", color=(1, 1, 0.2)))
    cyan_pigment = scene.add_pigment(Pigment("solid", color=(0.2, 1, 1)))
    magenta_pigment = scene.add_pigment(Pigment("solid", color=(1, 0.2, 1)))
    checker_pigment = scene.add_pigment(
        Pigment("checker", color1=(0.1, 0.1, 0.1), color2=(0.9, 0.9, 0.9), size=20)
    )

    # Finishes
    matte = scene.add_finish(SurfaceFinish(ka=0.3, kd=0.4, ks=0.0, alpha=1, kr=0.3))
    shiny = scene.add_finish(SurfaceFinish(ka=0.11, kd=0.11, ks=0.3, alpha=1000, kr=0.7))

    # Objects
    # Ground plane (large sphere)
    scene.add_object(
        Sphere(
            center=(0, -1000, 0),
            radius=1000,
            pigment_id=checker_pigment,
            finish_id=matte,
        )
    )

    # Row 1: Sphere in the back
    scene.add_object(
        Sphere(center=(0, 15, 50), radius=15, pigment_id=red_pigment, finish_id=shiny)
    )

    # Row 2: Different prisms (triangular, square, pentagonal, hexagonal)
    scene.add_object(
        Polyhedron.create_prism(
            center=(-45, 15, 20),
            radius=12,
            height=25,
            sides=3,
            pigment_id=blue_pigment,
            finish_id=matte,
        )
    )

    scene.add_object(
        Polyhedron.create_prism(
            center=(-15, 15, 20),
            radius=12,
            height=25,
            sides=4,
            pigment_id=green_pigment,
            finish_id=matte,
        )
    )

    scene.add_object(
        Polyhedron.create_prism(
            center=(15, 15, 20),
            radius=12,
            height=25,
            sides=5,
            pigment_id=yellow_pigment,
            finish_id=matte,
        )
    )

    scene.add_object(
        Polyhedron.create_prism(
            center=(45, 15, 20),
            radius=12,
            height=25,
            sides=6,
            pigment_id=cyan_pigment,
            finish_id=matte,
        )
    )

    # Row 3: Different pyramids (triangular, square, pentagonal, hexagonal)
    scene.add_object(
        Polyhedron.create_pyramid(
            base_center=(-45, 0, 0),
            radius=15,
            height=30,
            sides=3,
            pigment_id=magenta_pigment,
            finish_id=shiny,
        )
    )

    scene.add_object(
        Polyhedron.create_pyramid(
            base_center=(-15, 0, 0),
            radius=15,
            height=30,
            sides=4,
            pigment_id=red_pigment,
            finish_id=shiny,
        )
    )

    scene.add_object(
        Polyhedron.create_pyramid(
            base_center=(15, 0, 0),
            radius=15,
            height=30,
            sides=5,
            pigment_id=blue_pigment,
            finish_id=shiny,
        )
    )

    scene.add_object(
        Polyhedron.create_pyramid(
            base_center=(45, 0, 0),
            radius=15,
            height=30,
            sides=6,
            pigment_id=green_pigment,
            finish_id=shiny,
        )
    )

    # Cube in the back
    scene.add_object(
        Polyhedron.create_cube(
            center=(0, 15, 30), size=20, pigment_id=yellow_pigment, finish_id=matte
        )
    )

    return scene


def create_solar_system_scene():
    """Create a solar system inspired scene"""
    scene = Scene()
    
    # Camera
    scene.camera = Camera(
        position=(0, 100, -300), target=(0, 0, 0), normal=(0, 1, 0), fov=50
    )
    
    # Sun light
    scene.add_light(Light(position=(0, 0, 0), color=(1, 1, 0.9), attenuation=(1, 0, 0.0001)))
    
    # Pigments - planets
    sun_pigment = scene.add_pigment(Pigment("solid", color=(1, 0.9, 0.2)))
    mercury_pigment = scene.add_pigment(Pigment("solid", color=(0.7, 0.7, 0.7)))
    venus_pigment = scene.add_pigment(Pigment("solid", color=(0.9, 0.8, 0.5)))
    earth_pigment = scene.add_pigment(Pigment("solid", color=(0.2, 0.4, 0.8)))
    mars_pigment = scene.add_pigment(Pigment("solid", color=(0.8, 0.3, 0.2)))
    jupiter_pigment = scene.add_pigment(Pigment("solid", color=(0.8, 0.6, 0.4)))
    saturn_pigment = scene.add_pigment(Pigment("solid", color=(0.9, 0.8, 0.6)))
    space_pigment = scene.add_pigment(Pigment("solid", color=(0.05, 0.05, 0.1)))
    
    # Finishes
    glowing = scene.add_finish(SurfaceFinish(ka=0.9, kd=0.1, ks=0.0, alpha=1))
    matte = scene.add_finish(SurfaceFinish(ka=0.3, kd=0.7, ks=0.0, alpha=1))
    shiny = scene.add_finish(SurfaceFinish(ka=0.2, kd=0.5, ks=0.3, alpha=50, kr=0.2))
    
    # Space background (large sphere)
    scene.add_object(Sphere(center=(0, 0, 0), radius=2000, 
                           pigment_id=space_pigment, finish_id=matte))
    
    # Sun (center)
    scene.add_object(Sphere(center=(0, 0, 0), radius=30,
                           pigment_id=sun_pigment, finish_id=glowing))
    
    # Planets (orbital distances scaled down)
    scene.add_object(Sphere(center=(60, 0, 0), radius=5,
                           pigment_id=mercury_pigment, finish_id=matte))
    scene.add_object(Sphere(center=(90, 0, 20), radius=8,
                           pigment_id=venus_pigment, finish_id=matte))
    scene.add_object(Sphere(center=(120, 0, -30), radius=8,
                           pigment_id=earth_pigment, finish_id=shiny))
    scene.add_object(Sphere(center=(150, 0, 10), radius=6,
                           pigment_id=mars_pigment, finish_id=matte))
    scene.add_object(Sphere(center=(200, 0, -20), radius=18,
                           pigment_id=jupiter_pigment, finish_id=shiny))
    scene.add_object(Sphere(center=(250, 0, 15), radius=16,
                           pigment_id=saturn_pigment, finish_id=shiny))
    
    return scene


def create_chess_scene():
    """Create a chess board scene"""
    scene = Scene()
    
    # Camera
    scene.camera = Camera(
        position=(80, 60, -80), target=(0, 5, 0), normal=(0, 1, 0), fov=45
    )
    
    # Lights
    scene.add_light(Light(position=(50, 100, -50), color=(1, 1, 1)))
    scene.add_light(Light(position=(-50, 80, -50), color=(0.6, 0.6, 0.8)))
    
    # Pigments
    white_pigment = scene.add_pigment(Pigment("solid", color=(0.9, 0.9, 0.9)))
    black_pigment = scene.add_pigment(Pigment("solid", color=(0.1, 0.1, 0.1)))
    board_pigment = scene.add_pigment(Pigment("checker", 
                                              color1=(0.2, 0.15, 0.1),
                                              color2=(0.9, 0.85, 0.8),
                                              size=10))
    
    # Finishes
    wood = scene.add_finish(SurfaceFinish(ka=0.3, kd=0.6, ks=0.1, alpha=10))
    glossy = scene.add_finish(SurfaceFinish(ka=0.2, kd=0.4, ks=0.4, alpha=80, kr=0.3))
    
    # Chess board (large flat box)
    scene.add_object(Polyhedron.create_rectangular_prism(
        center=(0, -2, 0), width=80, height=2, depth=80,
        pigment_id=board_pigment, finish_id=wood
    ))
    
    # White pieces (pawns and rooks)
    for i in range(8):
        x = -35 + i * 10
        # Pawns (small cylinders approximated as prisms)
        scene.add_object(Polyhedron.create_prism(
            center=(x, 5, -20), radius=3, height=8, sides=8,
            pigment_id=white_pigment, finish_id=glossy
        ))
    
    # Rooks (corners)
    scene.add_object(Polyhedron.create_prism(
        center=(-35, 6, -30), radius=4, height=10, sides=4,
        pigment_id=white_pigment, finish_id=glossy
    ))
    scene.add_object(Polyhedron.create_prism(
        center=(35, 6, -30), radius=4, height=10, sides=4,
        pigment_id=white_pigment, finish_id=glossy
    ))
    
    # Black pieces
    for i in range(8):
        x = -35 + i * 10
        scene.add_object(Polyhedron.create_prism(
            center=(x, 5, 20), radius=3, height=8, sides=8,
            pigment_id=black_pigment, finish_id=glossy
        ))
    
    scene.add_object(Polyhedron.create_prism(
        center=(-35, 6, 30), radius=4, height=10, sides=4,
        pigment_id=black_pigment, finish_id=glossy
    ))
    scene.add_object(Polyhedron.create_prism(
        center=(35, 6, 30), radius=4, height=10, sides=4,
        pigment_id=black_pigment, finish_id=glossy
    ))
    
    return scene


def create_temple_scene():
    """Create an ancient temple scene"""
    scene = Scene()
    
    # Camera
    scene.camera = Camera(
        position=(0, 40, -120), target=(0, 20, 0), normal=(0, 1, 0), fov=50
    )
    
    # Lights
    scene.add_light(Light(position=(80, 100, -80), color=(1, 0.95, 0.8)))
    scene.add_light(Light(position=(-60, 80, -60), color=(0.5, 0.5, 0.6)))
    
    # Pigments
    stone_pigment = scene.add_pigment(Pigment("solid", color=(0.6, 0.55, 0.5)))
    gold_pigment = scene.add_pigment(Pigment("solid", color=(0.9, 0.75, 0.2)))
    floor_pigment = scene.add_pigment(Pigment("checker",
                                              color1=(0.4, 0.35, 0.3),
                                              color2=(0.5, 0.45, 0.4),
                                              size=15))
    
    # Finishes
    stone_finish = scene.add_finish(SurfaceFinish(ka=0.4, kd=0.6, ks=0.0, alpha=1))
    gold_finish = scene.add_finish(SurfaceFinish(ka=0.3, kd=0.4, ks=0.3, alpha=50, kr=0.4))
    
    # Floor
    scene.add_object(Sphere(center=(0, -1000, 0), radius=1000,
                           pigment_id=floor_pigment, finish_id=stone_finish))
    
    # Temple base
    scene.add_object(Polyhedron.create_rectangular_prism(
        center=(0, 2, 0), width=100, height=4, depth=80,
        pigment_id=stone_pigment, finish_id=stone_finish
    ))
    
    # Columns (hexagonal prisms)
    column_positions = [(-30, 15, -20), (0, 15, -20), (30, 15, -20),
                       (-30, 15, 20), (0, 15, 20), (30, 15, 20)]
    for pos in column_positions:
        scene.add_object(Polyhedron.create_prism(
            center=pos, radius=5, height=30, sides=6,
            pigment_id=stone_pigment, finish_id=stone_finish
        ))
    
    # Roof (pyramid on top)
    scene.add_object(Polyhedron.create_pyramid(
        base_center=(0, 30, 0), radius=55, height=25, sides=4,
        pigment_id=stone_pigment, finish_id=stone_finish
    ))
    
    # Golden sphere at apex
    scene.add_object(Sphere(center=(0, 57, 0), radius=8,
                           pigment_id=gold_pigment, finish_id=gold_finish))
    
    return scene


def create_crystal_garden_scene():
    """Create a garden of crystal formations"""
    scene = Scene()
    
    # Camera
    scene.camera = Camera(
        position=(50, 35, -80), target=(0, 15, 0), normal=(0, 1, 0), fov=55
    )
    
    # Lights
    scene.add_light(Light(position=(60, 80, -60), color=(1, 1, 1)))
    scene.add_light(Light(position=(-40, 60, -40), color=(0.7, 0.8, 1)))
    scene.add_light(Light(position=(0, 50, 50), color=(1, 0.7, 0.9)))
    
    # Pigments - crystals
    crystal_red = scene.add_pigment(Pigment("solid", color=(1, 0.3, 0.3)))
    crystal_blue = scene.add_pigment(Pigment("solid", color=(0.3, 0.5, 1)))
    crystal_green = scene.add_pigment(Pigment("solid", color=(0.3, 1, 0.5)))
    crystal_purple = scene.add_pigment(Pigment("solid", color=(0.8, 0.3, 1)))
    crystal_cyan = scene.add_pigment(Pigment("solid", color=(0.3, 1, 1)))
    ground_pigment = scene.add_pigment(Pigment("solid", color=(0.15, 0.12, 0.18)))
    
    # Finishes
    crystal_finish = scene.add_finish(SurfaceFinish(ka=0.2, kd=0.3, ks=0.5, alpha=100, kr=0.6))
    ground_finish = scene.add_finish(SurfaceFinish(ka=0.3, kd=0.7, ks=0.0, alpha=1))
    
    # Ground
    scene.add_object(Sphere(center=(0, -1000, 0), radius=1000,
                           pigment_id=ground_pigment, finish_id=ground_finish))
    
    # Crystal formations (pyramids with different bases)
    crystals = [
        # (x, y, z, radius, height, sides, pigment)
        (0, 0, 0, 8, 30, 3, crystal_red),
        (-25, 0, 10, 10, 35, 4, crystal_blue),
        (20, 0, -15, 7, 28, 5, crystal_green),
        (-15, 0, -25, 9, 32, 6, crystal_purple),
        (30, 0, 20, 6, 25, 3, crystal_cyan),
        (15, 0, 25, 8, 30, 4, crystal_red),
        (-30, 0, -10, 7, 27, 5, crystal_blue),
        (10, 0, -30, 9, 33, 3, crystal_green),
        (-5, 0, 20, 6, 24, 6, crystal_purple),
        (25, 0, 5, 8, 29, 4, crystal_cyan),
    ]
    
    for x, y, z, radius, height, sides, pigment in crystals:
        scene.add_object(Polyhedron.create_pyramid(
            base_center=(x, y, z), radius=radius, height=height, sides=sides,
            pigment_id=pigment, finish_id=crystal_finish
        ))
    
    return scene


def create_cityscape_scene():
    """Create a simple cityscape"""
    scene = Scene()
    
    # Camera
    scene.camera = Camera(
        position=(100, 50, -150), target=(0, 20, 0), normal=(0, 1, 0), fov=50
    )
    
    # Lights (sun and ambient)
    scene.add_light(Light(position=(200, 300, -200), color=(1, 0.95, 0.8)))
    scene.add_light(Light(position=(-100, 100, -100), color=(0.4, 0.4, 0.5)))
    
    # Pigments
    concrete = scene.add_pigment(Pigment("solid", color=(0.5, 0.5, 0.5)))
    glass_blue = scene.add_pigment(Pigment("solid", color=(0.3, 0.4, 0.6)))
    brick_red = scene.add_pigment(Pigment("solid", color=(0.6, 0.3, 0.2)))
    street = scene.add_pigment(Pigment("checker",
                                       color1=(0.2, 0.2, 0.2),
                                       color2=(0.3, 0.3, 0.3),
                                       size=20))
    
    # Finishes
    matte = scene.add_finish(SurfaceFinish(ka=0.3, kd=0.7, ks=0.0, alpha=1))
    glass = scene.add_finish(SurfaceFinish(ka=0.1, kd=0.2, ks=0.7, alpha=200, kr=0.5))
    
    # Street
    scene.add_object(Sphere(center=(0, -1000, 0), radius=1000,
                           pigment_id=street, finish_id=matte))
    
    # Buildings (various heights and styles)
    buildings = [
        # (x, z, width, height, depth, pigment, finish)
        (-40, 0, 20, 60, 20, concrete, matte),
        (-10, 0, 15, 45, 15, glass_blue, glass),
        (15, 0, 25, 80, 25, concrete, matte),
        (50, 0, 18, 55, 18, brick_red, matte),
        (-30, 35, 12, 35, 12, glass_blue, glass),
        (30, 30, 15, 40, 15, concrete, matte),
        (-50, -30, 22, 50, 22, brick_red, matte),
        (0, -35, 20, 70, 20, glass_blue, glass),
        (40, -30, 16, 45, 16, concrete, matte),
    ]
    
    for x, z, width, height, depth, pigment, finish in buildings:
        scene.add_object(Polyhedron.create_rectangular_prism(
            center=(x, height/2, z), width=width, height=height, depth=depth,
            pigment_id=pigment, finish_id=finish
        ))
    
    return scene


def main():
    parser = argparse.ArgumentParser(description="Scene Builder for Raytracer")
    parser.add_argument(
        "-i", "--interactive", action="store_true", help="Run in interactive mode"
    )
    parser.add_argument(
        "-e",
        "--example",
        type=str,
        choices=['gallery', 'solar', 'chess', 'temple', 'crystals', 'city'],
        help="Create example scene: gallery (all shapes), solar (solar system), chess (chess board), temple (ancient temple), crystals (crystal garden), city (cityscape)",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        help="Output filename (default: based on example type)",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Generate all example scenes",
    )

    args = parser.parse_args()

    if args.all:
        # Generate all example scenes
        examples = {
            'scene_gallery': create_example_scene,
            'scene_solar_system': create_solar_system_scene,
            'scene_chess': create_chess_scene,
            'scene_temple': create_temple_scene,
            'scene_crystals': create_crystal_garden_scene,
            'scene_cityscape': create_cityscape_scene,
        }
        
        for filename, create_func in examples.items():
            scene = create_func()
            filepath = os.path.join("data", "scenes", filename + ".txt")
            scene.save(filepath)
        
        print(f"\nAll {len(examples)} example scenes created in data/scenes/ directory")
        
    elif args.example:
        # Map example names to functions
        examples = {
            'gallery': create_example_scene,
            'solar': create_solar_system_scene,
            'chess': create_chess_scene,
            'temple': create_temple_scene,
            'crystals': create_crystal_garden_scene,
            'city': create_cityscape_scene,
        }
        
        scene = examples[args.example]()
        
        if args.output:
            filename = args.output
        else:
            # Default filenames
            default_names = {
                'gallery': 'scene_gallery',
                'solar': 'scene_solar_system',
                'chess': 'scene_chess',
                'temple': 'scene_temple',
                'crystals': 'scene_crystals',
                'city': 'scene_cityscape',
            }
            filename = default_names[args.example]
        
        if not filename.endswith(".txt"):
            filename += ".txt"
        filepath = os.path.join("data", "scenes", filename)
        scene.save(filepath)
        print(f"Example scene created: {filepath}")
        
    elif args.interactive:
        interactive_mode()
    else:
        # Default: show help and run interactive
        parser.print_help()
        print("\n" + "=" * 50)
        print("Starting interactive mode...")
        print("=" * 50 + "\n")
        interactive_mode()


if __name__ == "__main__":
    main()
