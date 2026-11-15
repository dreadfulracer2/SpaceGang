
#include "protocol/Protocol.h"

#include <cassert>
#include <iostream>

int main()
{
    Protocol::Message original = {
        'H', 'o', 'l', 'a'
    };

    auto encoded = Protocol::encode(original);

    Protocol::Message buffer;
    Protocol::Message decoded;

    // Simulamos que TCP entrega solo 3 bytes.
    buffer.insert(
        buffer.end(),
        encoded.begin(),
        encoded.begin() + 3
    );

    assert(!Protocol::decode(buffer, decoded));

    std::cout << "Primer fragmento recibido\n";
    std::cout << "Mensaje incompleto: correcto\n";

    // Llega el resto del mensaje.
    buffer.insert(
        buffer.end(),
        encoded.begin() + 3,
        encoded.end()
    );

    assert(Protocol::decode(buffer, decoded));
    assert(decoded == original);

    std::cout << "Segundo fragmento recibido\n";
    std::cout << "Mensaje reconstruido: ";

    for (auto byte : decoded)
        std::cout << static_cast<char>(byte);

    std::cout << "\n";

    return 0;
}
