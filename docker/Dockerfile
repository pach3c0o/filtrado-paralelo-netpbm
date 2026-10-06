# Nodo MPI: compilador, OpenMPI y ssh (mpirun lanza los procesos remotos por ssh).
FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends \
      g++ make openmpi-bin libopenmpi-dev openssh-server openssh-client \
    && rm -rf /var/lib/apt/lists/* && mkdir -p /run/sshd && ssh-keygen -A

# Todos los contenedores comparten la misma llave: ssh sin contraseña entre nodos.
RUN useradd -m -s /bin/bash mpi
USER mpi
RUN ssh-keygen -t ed25519 -N "" -f ~/.ssh/id_ed25519 \
    && cp ~/.ssh/id_ed25519.pub ~/.ssh/authorized_keys \
    && printf "Host *\n  StrictHostKeyChecking no\n  UserKnownHostsFile /dev/null\n  LogLevel ERROR\n" > ~/.ssh/config

WORKDIR /home/mpi/app
COPY --chown=mpi Makefile ./
COPY --chown=mpi headers headers
COPY --chown=mpi src src
RUN make mpi_filterer

USER root
CMD ["/usr/sbin/sshd", "-D"]
