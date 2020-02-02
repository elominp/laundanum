import { Entity, PrimaryGeneratedColumn, Column,
createConnection, Not, Equal } from 'typeorm';

export interface IToto {
    id: number;
    tata: string;
}

@Entity()
export class Toto implements IToto {
    @PrimaryGeneratedColumn()
    id: number;
    @Column()
    tata: string;
};

createConnection({ type: 'sqlite', database: 'toto.db', entities: [ Toto ]
}).then(async (db) => {
    var toto = new Toto();
    var repo = db.getRepository(Toto);
    toto.tata = 'Hello, World!';
    await repo.save(toto);
    var tata : IToto[] = await repo.find({
        tata: Not(Equal('Hello, World!'))
    });
    console.log(tata);
});